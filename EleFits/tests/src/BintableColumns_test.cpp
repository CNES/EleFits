// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableColumns.h"
#include "EleFits/TestBintable.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

// read_segment_to(rows, key, column)
//   read_to(key, column)
//     read_to(column) => TEST
//   read_segment(rows, key)
//     read(key) => TEST
//   read_segment_to(rows, column) => TEST
//
// read_n_segments_to (rows, keys, columns) -> loop on read_segment_to (rows, key, column)
//   read_n_to (keys, columns)
//     read_n (indices...)
//       read_n (names...) => SEQ_WRITE_READ_TEST
//     read_n_to (columns)
//       read_n_to (columns...) => TEST
//     read_n_to (keys, columns...) => TEST
//   read_n_segments (rows, indices...)
//     read_n_segments (rows, names...) => SEQ_WRITE_READ_TEST
//   read_n_segments_to (rows, keys, columns)
//     read_n_segments_to (rows, columns) => TEST
//       read_n_segments_to (rows, columns...) => TEST
//
// write_segment(rows, column)
//   write_segment(column)
//
// write_n_segments (first_row, columns) -> loop on write_segment (row, column)
//   write_n (columns) => SEQ_WRITE_READ_TEST
//     write_n (columns...) => SEQ_WRITE_READ_TEST
//   write_n_segments (first_row, columns...) => SEQ_WRITE_READ_TEST
//
// insert_n_null (index, infos) => SEQ_WRITE_READ_TEST
//   insert_n_null (index, infos...) => SEQ_WRITE_READ_TEST
//
// remove_n (keys) => SEQ_WRITE_READ_TEST

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableColumns_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(columns_row_count_test, Test::SmallTable)
{
  const auto row_count = nums.size();
  BOOST_TEST(names.size() == row_count);
  const auto columns = std::make_tuple(num_col, radec_col, name_col, dist_mag_col);
  BOOST_TEST(columns_row_count(columns) == static_cast<long>(row_count));
}

void check_insert_column(MefFile& f, long index)
{
  const Test::SmallTable table;
  const auto& ext = f.append_bintable("TABLE", {}, table.name_col, table.radec_col);
  const auto& columns = ext.columns();
  const auto init_size = columns.read_column_count();
  BOOST_TEST(not columns.has("COL"));
  if (index == -1) {
    columns.append_null(ColumnInfo<char>("COL"));
  } else {
    columns.insert_null(index, ColumnInfo<char>("COL"));
  }
  BOOST_TEST(columns.read_column_count() == init_size + 1);
  BOOST_TEST(columns.has("COL"));
  if (index == -1) {
    BOOST_TEST(columns.read_index("COL") == init_size);
  } else {
    BOOST_TEST(columns.read_index("COL") == index);
  }
  const auto col = columns.read<char>("COL");
  for (auto e : col) {
    BOOST_TEST(e == -128); // BZERO
  }
}

BOOST_FIXTURE_TEST_CASE(prepend_column_test, Test::TemporaryMefFile)
{
  check_insert_column(*this, 0);
}

BOOST_FIXTURE_TEST_CASE(insert_column_test, Test::TemporaryMefFile)
{
  check_insert_column(*this, 2);
}

BOOST_FIXTURE_TEST_CASE(append_column_test, Test::TemporaryMefFile)
{
  check_insert_column(*this, -1);
}

BOOST_FIXTURE_TEST_CASE(append_rows_test, Test::TemporaryMefFile)
{
  const Test::SmallTable table;
  const auto init_size = static_cast<long>(table.names.size());
  const auto& ext = append_bintable("TABLE", {}, table.name_col, table.radec_col);
  const auto& columns = ext.columns();
  BOOST_TEST(columns.read_row_count() == init_size);
  columns.write_n_segments(-1, table.name_col, table.radec_col);
  BOOST_TEST(columns.read_row_count() == init_size * 2);
}

template <typename T>
void check_tuple_write_read(const BintableColumns& du, const VecColumn<T>& first, const VecColumn<T>& last)
{
  /* Write */
  const auto row_count = first.row_count();
  du.write_n(last, first);
  BOOST_TEST(du.read_row_count() == row_count);

  /* Read */
  const auto [res0, res1] = du.read_n(as<T>(last.info().name), as<T>(first.info().name)); // Structured binding
  BOOST_TEST((res0.info() == last.info()));
  BOOST_TEST((res1.info() == first.info()));
  BOOST_TEST(res0.container() == last.container());
  BOOST_TEST(res1.container() == first.container());

  /* Append */
  du.write_n_segments(-1, last, first);
  BOOST_TEST(du.read_row_count() == row_count * 2);

  /* Read */
  const auto res2 = du.read_n_segments({row_count, -1}, as<T>(last.info().name), as<T>(first.info().name));
  const auto& res20 = std::get<0>(res2);
  const auto& res21 = std::get<1>(res2);
  BOOST_TEST((res20.info() == last.info()));
  BOOST_TEST((res21.info() == first.info()));
  BOOST_TEST(res20.container() == last.container());
  BOOST_TEST(res21.container() == first.container());
}

template <>
void check_tuple_write_read<std::string>(
    const BintableColumns& du,
    const VecColumn<std::string>& first,
    const VecColumn<std::string>& last)
{
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
  (void)first;
  (void)last;
}

template <>
void check_tuple_write_read<std::uint64_t>(
    const BintableColumns& du,
    const VecColumn<std::uint64_t>& first,
    const VecColumn<std::uint64_t>& last)
{
  // FIXME CFITSIO bug, see below
  (void)du;
  (void)first;
  (void)last;
}

template <typename T>
void check_array_write_read(const BintableColumns& du)
{
  /* Generate */
  const long row_count = 10000;
  const long repeat_count = 3;
  std::array<ColumnInfo<T>, 2> infos {
      ColumnInfo<T> {"VECTOR", "m", repeat_count},
      ColumnInfo<T> {"SCALAR", "s", 1}}; // Inverted for robustness test
  std::array<VecColumn<T>, 2> seq {
      VecColumn<T> {infos[1], Test::generate_random_vector<T>(row_count)},
      VecColumn<T> {infos[0], Test::generate_random_vector<T>(repeat_count * row_count)}};

  /* Write */
  du.insert_n_null(0, infos);
  du.write_n(seq);
  const auto res = du.read_n<T>({0, 1});

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.container() == seq[1].container());
  BOOST_TEST(res1.container() == seq[0].container());

  /* Remove */
  du.remove_n({1, 0}); // Inverted for robustness test
  BOOST_TEST(not du.has(infos[0].name)); // FIXME accept ColumnInfo as key
  BOOST_TEST(not du.has(infos[1].name)); // FIXME idem
}

template <>
void check_array_write_read<std::string>(const BintableColumns& du)
{
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

/**
 * @brief Show CFITSIO bug when inserting a uint64 column.
 */
template <>
void check_array_write_read<std::uint64_t>(const BintableColumns& /* du */)
{
  /* Setup */
  using T = std::uint64_t;
  Test::TemporaryMefFile f;
  fitsfile* fptr = f.handover_to_cfitsio(); // FIXME in FitsFile, explicitely keep filename() available
  int status = 0;

  /* Create ext */
  std::string ttype_str = "SCALAR";
  auto ttype_ptr = String::to_char_ptr(ttype_str);
  auto* ttype = ttype_ptr.get();
  std::string tform_str = Euclid::Cfitsio::TypeCode<T>::tform(1); // 1W
  auto tform_ptr = String::to_char_ptr(tform_str);
  auto* tform = tform_ptr.get();
  // fits_create_tbl(fptr, BINARY_TBL, 0, 1, &ttype, &tform, nullptr, "CFITSIO", &status); // Works this way...
  fits_create_tbl(fptr, BINARY_TBL, 0, 0, nullptr, nullptr, nullptr, "CFITSIO", &status); // But not this way!
  fits_insert_col(fptr, 1, ttype, tform, &status);
  BOOST_TEST(status == 0);

  /* Write data */
  constexpr long row_count = 10000;
  auto data = Test::generate_random_vector<T>(row_count);
  fits_write_col(fptr, Euclid::Cfitsio::TypeCode<T>::for_bintable(), 1, 1, 1, row_count, data.data(), &status);
  BOOST_TEST(status == BAD_BTABLE_FORMAT); // FIXME CFITSIO bug (works with all other types)

  /* Tear down */
  remove(f.filename().c_str());
}

template <typename T>
void check_vector_write_read(const BintableColumns& du)
{
  /* Generate */
  const long row_count = 10000;
  const long repeat_count = 3;
  std::vector<ColumnInfo<T>> infos {{"VECTOR", "m", repeat_count}, {"SCALAR", "s", 1}}; // Inverted for robustness test
  std::vector<VecColumn<T>> seq {
      {infos[1], Test::generate_random_vector<T>(row_count)},
      {infos[0], Test::generate_random_vector<T>(repeat_count * row_count)}};

  /* Write */
  du.insert_n_null(0, infos);
  du.write_n(seq);
  const auto res = du.read_n<T>({infos[0].name, infos[1].name}); // FIXME accept ColumnInfo as key

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.container() == seq[1].container());
  BOOST_TEST(res1.container() == seq[0].container());

  /* Remove */
  du.remove_n({infos[0].name, infos[1].name}); // Inverted for robustness test
  BOOST_TEST(not du.has(infos[0].name));
  BOOST_TEST(not du.has(infos[1].name));
}

template <>
void check_vector_write_read<std::string>(const BintableColumns& du)
{
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

template <>
void check_vector_write_read<std::uint64_t>(const BintableColumns& du)
{
  // FIXME CFITSIO bug, see above
  (void)du;
}

#define SEQ_WRITE_READ_TEST(type, name) \
  BOOST_FIXTURE_TEST_CASE(name##_tuple_write_read_test, Test::TestBintable<type>) \
  { \
    check_tuple_write_read<type>(columns, first_column, last_column); \
  } \
  BOOST_FIXTURE_TEST_CASE(name##_array_write_read_test, Test::TemporaryMefFile) \
  { \
    const auto& ext = this -> append_bintable_header("ARRAY"); \
    check_array_write_read<type>(ext.columns()); \
  } \
  BOOST_FIXTURE_TEST_CASE(name##_vector_write_read_test, Test::TemporaryMefFile) \
  { \
    const auto& ext = this -> append_bintable_header("VECTOR"); \
    check_vector_write_read<type>(ext.columns()); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(SEQ_WRITE_READ_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
