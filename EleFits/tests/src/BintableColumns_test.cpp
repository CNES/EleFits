/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "EleFits/BintableColumns.h"
#include "EleFits/FitsFileFixture.h"
#include "EleFitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

// Call graphs for sequences:
//
// readSegmentSeqTo (rows, indices, columns) -> loop on readSegmentTo (rows, index, column)
// 	readSeqTo (indices, columns)
// 		readSeq (indices...)
// 			readSeq (names...) => SEQ_WRITE_READ_TEST
// 		readSeqTo (names, columns)
// 			readSeqTo (columns)
// 				readSeqTo (columns...) => TEST
// 			readSeqTo (names, columns...) => TEST
// 		readSeqTo (indices, columns...) => TEST
// 	readSegmentSeq (rows, indices...)
// 		readSegmentSeq (rows, names...) => TEST
// 	readSegmentSeqTo (rows, names, columns)
// 		readSegmentSeqTo (rows, columns)
// 		readSegmentSeqTo (rows, names, columns...)
// 			readSegmentSeqTo (rows, columns...) => TEST
// 	readSegmentSeqTo (rows, indices, columns...) => TEST
//
// writeSegmentSeq (long firstRow, TSeq &&columns) -> loop on writeSegment (row, column)
//   writeSeq (TSeq &&columns) => SEQ_WRITE_READ_TEST
//     writeSeq (const Column< Ts > &... columns) => SEQ_WRITE_READ_TEST
//   writeSegmentSeq (long firstRow, Column< Ts > &... columns) => TEST
//
// initSeq (long index, TSeq &&infos) => SEQ_WRITE_READ_TEST
//   initSeq (long index, const ColumnInfo< Ts > &... infos) => SEQ_WRITE_READ_TEST
//
// removeSeq (const std::vector< long > &indices)
//   removeSeq (const std::vector< std::string > &names) => TEST

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(BintableColumns_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(columns_row_count_test, Test::SmallTable) {
  const auto rowCount = nums.size();
  BOOST_TEST(names.size() == rowCount);
  const auto columns = std::make_tuple(numCol, radecCol, nameCol, distMagCol);
  BOOST_TEST(columnsRowCount(columns) == rowCount);
}

BOOST_FIXTURE_TEST_CASE(append_rows_test, Test::TemporaryMefFile) {
  const Test::SmallTable table;
  const auto initSize = table.names.size();
  const auto& ext = assignBintableExt("TABLE", table.nameCol, table.radecCol);
  const auto& columns = ext.columns();
  BOOST_TEST(columns.readRowCount() == initSize);
  columns.writeSegmentSeq(-1, table.nameCol, table.radecCol);
  BOOST_TEST(columns.readRowCount() == initSize * 2);
}

template <typename T>
void checkTupleWriteRead(const BintableColumns& du) {
  const long rowCount = 10000;
  const long repeatCount = 3;
  Test::RandomScalarColumn<T> scalar(rowCount);
  Test::RandomVectorColumn<T> vector(repeatCount, rowCount);
  du.initSeq(0, vector.info(), scalar.info()); // Inverted for robustness test
  du.writeSeq(scalar, vector);
  BOOST_TEST(du.readRowCount() == rowCount);
  const auto res = du.readSeq(Named<T>(vector.info().name), Named<T>(scalar.info().name));
  const auto& res0 = std::get<0>(res);
  const auto& res1 = std::get<1>(res);
  BOOST_TEST((res0.info() == vector.info()));
  BOOST_TEST((res1.info() == scalar.info()));
  BOOST_TEST(res1.info().name == scalar.info().name);
  BOOST_TEST(res1.info().unit == scalar.info().unit);
  BOOST_TEST(res1.info().repeatCount == scalar.info().repeatCount);
  BOOST_TEST(res0.vector() == vector.vector());
  BOOST_TEST(res1.vector() == scalar.vector());
}

template <>
void checkTupleWriteRead<std::string>(const BintableColumns& du) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

template <>
void checkTupleWriteRead<std::uint64_t>(const BintableColumns& du) {
  // FIXME CFitsIO bug?
  (void)du;
}

template <typename T>
void checkArrayWriteRead(const BintableColumns& du) {
  const long rowCount = 10000;
  const long repeatCount = 3;
  std::array<ColumnInfo<T>, 2> infos { ColumnInfo<T> { "VECTOR", "m", repeatCount },
                                       ColumnInfo<T> { "SCALAR", "s", 1 } }; // Inverted for robustness test
  std::array<VecColumn<T>, 2> seq { VecColumn<T> { infos[1], Test::generateRandomVector<T>(rowCount) },
                                    VecColumn<T> { infos[0], Test::generateRandomVector<T>(repeatCount * rowCount) } };
  du.initSeq(0, infos);
  du.writeSeq(seq);
  const auto res = du.readSeq(Indexed<T>(0), Indexed<T>(1)); // TODO readSeq<T>({ 0, 1 }) and idem with strings?
  const auto& res0 = std::get<0>(res);
  const auto& res1 = std::get<1>(res);
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.vector() == seq[1].vector());
  BOOST_TEST(res1.vector() == seq[0].vector());
}

template <>
void checkArrayWriteRead<std::string>(const BintableColumns& du) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

template <>
void checkArrayWriteRead<std::uint64_t>(const BintableColumns& du) {
  // FIXME CFitsIO bug?
  (void)du;
}

template <typename T>
void checkVectorWriteRead(const BintableColumns& du) {
  const long rowCount = 10000;
  const long repeatCount = 3;
  std::vector<ColumnInfo<T>> infos { { "VECTOR", "m", repeatCount },
                                     { "SCALAR", "s", 1 } }; // Inverted for robustness test
  std::vector<VecColumn<T>> seq { { infos[1], Test::generateRandomVector<T>(rowCount) },
                                  { infos[0], Test::generateRandomVector<T>(repeatCount * rowCount) } };
  du.initSeq(0, infos);
  du.writeSeq(seq);
  const auto res = du.readSeq(Indexed<T>(0), Indexed<T>(1)); // TODO readSeq<T>({ 0, 1 }) and idem with strings?
  const auto& res0 = std::get<0>(res);
  const auto& res1 = std::get<1>(res);
  BOOST_TEST((res0.info() == seq[1].info()));
  BOOST_TEST((res1.info() == seq[0].info()));
  BOOST_TEST(res0.vector() == seq[1].vector());
  BOOST_TEST(res1.vector() == seq[0].vector());
}

template <>
void checkVectorWriteRead<std::string>(const BintableColumns& du) {
  // FIXME (cannot use RandomVectorColumn)
  (void)du;
}

template <>
void checkVectorWriteRead<std::uint64_t>(const BintableColumns& du) {
  // FIXME CFitsIO bug?
  (void)du;
}

#define SEQ_WRITE_READ_TEST(type, name) \
  BOOST_FIXTURE_TEST_CASE(name##_tuple_write_read_test, Test::TemporaryMefFile) { \
    const auto& ext = this->initBintableExt("TUPLE"); \
    checkTupleWriteRead<type>(ext.columns()); \
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
