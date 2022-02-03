// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableColumns.h"
#include "EleFits/TestBintable.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

// readSegmentTo(rows, key, column)
//   readTo(key, column)
//     readTo(column) => TEST
//   readSegment(rows, key)
//     read(key) => TEST
//   readSegmentTo(rows, column) => TEST
//
// readSegmentSeqTo (rows, keys, columns) -> loop on readSegmentTo (rows, key, column)
//   readSeqTo (keys, columns)
//     readSeq (indices...)
//       readSeq (names...) => SEQ_WRITE_READ_TEST
//     readSeqTo (columns)
//       readSeqTo (columns...) => TEST
//     readSeqTo (keys, columns...) => TEST
//   readSegmentSeq (rows, indices...)
//     readSegmentSeq (rows, names...) => SEQ_WRITE_READ_TEST
//   readSegmentSeqTo (rows, keys, columns)
//     readSegmentSeqTo (rows, columns) => TEST
//       readSegmentSeqTo (rows, columns...) => TEST
//
// writeSegment(rows, column)
//   writeSegment(column)
//
// writeSegmentSeq (firstRow, columns) -> loop on writeSegment (row, column)
//   writeSeq (columns) => SEQ_WRITE_READ_TEST
//     writeSeq (columns...) => SEQ_WRITE_READ_TEST
//   writeSegmentSeq (firstRow, columns...) => SEQ_WRITE_READ_TEST
//
// initSeq (index, infos) => SEQ_WRITE_READ_TEST
//   initSeq (index, infos...) => SEQ_WRITE_READ_TEST
//
// removeSeq (keys) => SEQ_WRITE_READ_TEST

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableColumns_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(columns_row_count_test, Test::SmallTable) {
  const auto rowCount = nums.size();
  BOOST_TEST(names.size() == rowCount);
  const auto columns = std::make_tuple(numCol, radecCol, nameCol, distMagCol);
  BOOST_TEST(columnsRowCount(columns) == static_cast<long>(rowCount));
}

BOOST_FIXTURE_TEST_CASE(append_rows_test, Test::TemporaryMefFile) {
  const Test::SmallTable table;
  const auto initSize = static_cast<long>(table.names.size());
  const auto& ext = assignBintableExt("TABLE", table.nameCol, table.radecCol);
  const auto& columns = ext.columns();
  BOOST_TEST(columns.readRowCount() == initSize);
  columns.writeSegmentSeq(-1, table.nameCol, table.radecCol);
  BOOST_TEST(columns.readRowCount() == initSize * 2);
}

template <typename T>
void checkTupleWriteRead(const BintableColumns& du, const VecColumn<T>& first, const VecColumn<T>& last) {

  /* Write */
  const auto rowCount = first.rowCount();
  du.writeSeq(last, first);
  BOOST_TEST(du.readRowCount() == rowCount);

  /* Read */
  const auto res = du.readSeq(as<T>(last.info().name), as<T>(first.info().name));
  const auto& res0 = std::get<0>(res);
  const auto& res1 = std::get<1>(res);
  BOOST_TEST((res0.info() == last.info()));
  BOOST_TEST((res1.info() == first.info()));
  BOOST_TEST(res0.vector() == last.vector());
  BOOST_TEST(res1.vector() == first.vector());

  /* Append */
  du.writeSegmentSeq(-1, last, first);
  BOOST_TEST(du.readRowCount() == rowCount * 2);

  /* Read */
  const auto res2 = du.readSegmentSeq({rowCount, -1}, as<T>(last.info().name), as<T>(first.info().name));
  const auto& res20 = std::get<0>(res2);
  const auto& res21 = std::get<1>(res2);
  BOOST_TEST((res20.info() == last.info()));
  BOOST_TEST((res21.info() == first.info()));
  BOOST_TEST(res20.vector() == last.vector());
  BOOST_TEST(res21.vector() == first.vector());
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
  const long rowCount = 10000;
  const long repeatCount = 3;
  std::array<ColumnInfo<T>, 2> infos {
      ColumnInfo<T> {"VECTOR", "m", repeatCount},
      ColumnInfo<T> {"SCALAR", "s", 1}}; // Inverted for robustness test
  std::array<VecColumn<T>, 2> seq {
      VecColumn<T> {infos[1], Test::generateRandomVector<T>(rowCount)},
      VecColumn<T> {infos[0], Test::generateRandomVector<T>(repeatCount * rowCount)}};

  /* Write */
  du.initSeq(0, infos);
  du.writeSeq(seq);
  const auto res = du.readSeq<T>({0, 1});

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.vector() == seq[1].vector());
  BOOST_TEST(res1.vector() == seq[0].vector());

  /* Remove */
  du.removeSeq({1, 0}); // Inverted for robustness test
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
void checkArrayWriteRead<std::uint64_t>(const BintableColumns& du) {

  /* Setup */
  using T = std::uint64_t;
  Test::TemporaryMefFile f;
  fitsfile* fptr = f.handoverToCfitsio(); // FIXME in FitsFile, explicitely keep filename() available
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
  constexpr long rowCount = 10000;
  auto data = Test::generateRandomVector<T>(rowCount);
  fits_write_col(fptr, Euclid::Cfitsio::TypeCode<T>::forBintable(), 1, 1, 1, rowCount, data.data(), &status);
  BOOST_TEST(status == BAD_BTABLE_FORMAT); // FIXME CFITSIO bug (works with all other types)

  /* Tear down */
  remove(f.filename().c_str());
}

template <typename T>
void checkVectorWriteRead(const BintableColumns& du) {

  /* Generate */
  const long rowCount = 10000;
  const long repeatCount = 3;
  std::vector<ColumnInfo<T>> infos {{"VECTOR", "m", repeatCount}, {"SCALAR", "s", 1}}; // Inverted for robustness test
  std::vector<VecColumn<T>> seq {
      {infos[1], Test::generateRandomVector<T>(rowCount)},
      {infos[0], Test::generateRandomVector<T>(repeatCount * rowCount)}};

  /* Write */
  du.initSeq(0, infos);
  du.writeSeq(seq);
  const auto res = du.readSeq<T>({infos[0].name, infos[1].name}); // FIXME accept ColumnInfo as key

  /* Read */
  const auto& res0 = res[0];
  const auto& res1 = res[1];
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.vector() == seq[1].vector());
  BOOST_TEST(res1.vector() == seq[0].vector());

  /* Remove */
  du.removeSeq({infos[0].name, infos[1].name}); // Inverted for robustness test
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
    const auto& ext = this->initBintableExt("ARRAY"); \
    checkArrayWriteRead<type>(ext.columns()); \
  } \
  BOOST_FIXTURE_TEST_CASE(name##_vector_write_read_test, Test::TemporaryMefFile) { \
    const auto& ext = this->initBintableExt("VECTOR"); \
    checkVectorWriteRead<type>(ext.columns()); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(SEQ_WRITE_READ_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
