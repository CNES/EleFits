// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableColumns.h"
#include "EleFits/TestBintable.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

// readSegmentTo(rows, key, column)
//   read_to(key, column)
//     read_to(column) => TEST
//   readSegment(rows, key)
//     read(key) => TEST
//   readSegmentTo(rows, column) => TEST
//
// readSegmentSeqTo (rows, keys, columns) -> loop on readSegmentTo (rows, key, column)
//   read_seq_to (keys, columns)
//     read_seq (indices...)
//       read_seq (names...) => SEQ_WRITE_READ_TEST
//     read_seq_to (columns)
//       read_seq_to (columns...) => TEST
//     read_seq_to (keys, columns...) => TEST
//   read_segment_seq (rows, indices...)
//     read_segment_seq (rows, names...) => SEQ_WRITE_READ_TEST
//   readSegmentSeqTo (rows, keys, columns)
//     readSegmentSeqTo (rows, columns) => TEST
//       readSegmentSeqTo (rows, columns...) => TEST
//
// write_segment(rows, column)
//   write_segment(column)
//
// write_segment_seq (firstRow, columns) -> loop on write_segment (row, column)
//   write_seq (columns) => SEQ_WRITE_READ_TEST
//     write_seq (columns...) => SEQ_WRITE_READ_TEST
//   write_segment_seq (firstRow, columns...) => SEQ_WRITE_READ_TEST
//
// insert_null_seq (index, infos) => SEQ_WRITE_READ_TEST
//   insert_null_seq (index, infos...) => SEQ_WRITE_READ_TEST
//
// remove_seq (keys) => SEQ_WRITE_READ_TEST

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableColumns_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(columns_row_count_test, Test::SmallTable) {
  const auto row_count = nums.size();
  BOOST_TEST(names.size() == row_count);
  const auto columns = std::make_tuple(num_col, radec_col, name_col, dist_mag_col);
  BOOST_TEST(columns_row_count(columns) == static_cast<long>(row_count));
}

BOOST_FIXTURE_TEST_CASE(append_rows_test, Test::TemporaryMefFile) {
  const Test::SmallTable table;
  const auto initSize = static_cast<long>(table.names.size());
  const auto& ext = append_bintable("TABLE", {}, table.name_col, table.radec_col);
  const auto& columns = ext.columns();
  BOOST_TEST(columns.read_row_count() == initSize);
  columns.write_segment_seq(-1, table.name_col, table.radec_col);
  BOOST_TEST(columns.read_row_count() == initSize * 2);
}

template <typename T>
void checkTupleWriteRead(const BintableColumns& du, const VecColumn<T>& first, const VecColumn<T>& last) {

  /* Write */
  const auto row_count = first.row_count();
  du.write_seq(last, first);
  BOOST_TEST(du.read_row_count() == row_count);

  /* Read */
  const auto [res0, res1] = du.read_seq(as<T>(last.info().name), as<T>(first.info().name)); // Structured binding
  BOOST_TEST((res0.info() == last.info()));
  BOOST_TEST((res1.info() == first.info()));
  BOOST_TEST(res0.container() == last.container());
  BOOST_TEST(res1.container() == first.container());

  /* Append */
  du.write_segment_seq(-1, last, first);
  BOOST_TEST(du.read_row_count() == row_count * 2);

  /* Read */
  const auto res2 = du.read_segment_seq({row_count, -1}, as<T>(last.info().name), as<T>(first.info().name));
  const auto& res20 = std::get<0>(res2);
  const auto& res21 = std::get<1>(res2);
  BOOST_TEST((res20.info() == last.info()));
  BOOST_TEST((res21.info() == first.info()));
  BOOST_TEST(res20.container() == last.container());
  BOOST_TEST(res21.container() == first.container());
}

template <>
void checkTupleWriteRead<std::string>(
    const BintableColumns& du,
    const VecColumn<std::string>& first,
    const VecColumn<std::string>& last) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
  (void)first;
  (void)last;
}

template <>
void checkTupleWriteRead<std::uint64_t>(
    const BintableColumns& du,
    const VecColumn<std::uint64_t>& first,
    const VecColumn<std::uint64_t>& last) {
  // FIXME CFITSIO bug, see below
  (void)du;
  (void)first;
  (void)last;
}

template <typename T>
void checkArrayWriteRead(const BintableColumns& du) {

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
  du.insert_null_seq(0, infos);
  du.write_seq(seq);
  const auto res = du.read_seq<T>({0, 1});

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.container() == seq[1].container());
  BOOST_TEST(res1.container() == seq[0].container());

  /* Remove */
  du.remove_seq({1, 0}); // Inverted for robustness test
  BOOST_TEST(not du.has(infos[0].name)); // FIXME accept ColumnInfo as key
  BOOST_TEST(not du.has(infos[1].name)); // FIXME idem
}

template <>
void checkArrayWriteRead<std::string>(const BintableColumns& du) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

/**
 * @brief Show CFITSIO bug when inserting a uint64 column.
 */
template <>
void checkArrayWriteRead<std::uint64_t>(const BintableColumns& /* du */) {

  /* Setup */
  using T = std::uint64_t;
  Test::TemporaryMefFile f;
  fitsfile* fptr = f.handover_to_cfitsio(); // FIXME in FitsFile, explicitely keep filename() available
  int status = 0;

  /* Create ext */
  std::string ttypeStr = "SCALAR";
  auto ttypePtr = String::toCharPtr(ttypeStr);
  auto* ttype = ttypePtr.get();
  std::string tformStr = Euclid::Cfitsio::TypeCode<T>::tform(1); // 1W
  auto tformPtr = String::toCharPtr(tformStr);
  auto* tform = tformPtr.get();
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
void checkVectorWriteRead(const BintableColumns& du) {

  /* Generate */
  const long row_count = 10000;
  const long repeat_count = 3;
  std::vector<ColumnInfo<T>> infos {{"VECTOR", "m", repeat_count}, {"SCALAR", "s", 1}}; // Inverted for robustness test
  std::vector<VecColumn<T>> seq {
      {infos[1], Test::generate_random_vector<T>(row_count)},
      {infos[0], Test::generate_random_vector<T>(repeat_count * row_count)}};

  /* Write */
  du.insert_null_seq(0, infos);
  du.write_seq(seq);
  const auto res = du.read_seq<T>({infos[0].name, infos[1].name}); // FIXME accept ColumnInfo as key

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.container() == seq[1].container());
  BOOST_TEST(res1.container() == seq[0].container());

  /* Remove */
  du.remove_seq({infos[0].name, infos[1].name}); // Inverted for robustness test
  BOOST_TEST(not du.has(infos[0].name));
  BOOST_TEST(not du.has(infos[1].name));
}

template <>
void checkVectorWriteRead<std::string>(const BintableColumns& du) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

template <>
void checkVectorWriteRead<std::uint64_t>(const BintableColumns& du) {
  // FIXME CFITSIO bug, see above
  (void)du;
}

#define SEQ_WRITE_READ_TEST(type, name) \
  BOOST_FIXTURE_TEST_CASE(name##_tuple_write_read_test, Test::TestBintable<type>) { \
    checkTupleWriteRead<type>(columns, firstColumn, lastColumn); \
  } \
  BOOST_FIXTURE_TEST_CASE(name##_array_write_read_test, Test::TemporaryMefFile) { \
    const auto& ext = this->append_bintable_header("ARRAY"); \
    checkArrayWriteRead<type>(ext.columns()); \
  } \
  BOOST_FIXTURE_TEST_CASE(name##_vector_write_read_test, Test::TemporaryMefFile) { \
    const auto& ext = this->append_bintable_header("VECTOR"); \
    checkVectorWriteRead<type>(ext.columns()); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(SEQ_WRITE_READ_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
