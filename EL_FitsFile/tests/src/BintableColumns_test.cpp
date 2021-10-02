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

#include "EL_FitsData/TestColumn.h"
#include "EL_FitsFile/BintableColumns.h"
#include "EL_FitsFile/FitsFileFixture.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

// Call graphs for sequences:
//
// readSegmentSeqTo (rows, indices, columns) -> loop on readSegmentTo (rows, index, column)
// 	readSeqTo (indices, columns)
// 		readSeq (indices...)
// 			readSeq (names...) => TEST
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
//   writeSeq (TSeq &&columns)
//     writeSeq (const Column< Ts > &... columns) => TEST
//   writeSegmentSeq (long firstRow, Column< Ts > &... columns) => TEST
//
// initSeq (TSeq &&infos, long index)
//   initSeq (const ColumnInfo< Ts > &... infos, long index) => TEST
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
  const auto columns = ext.columns();
  BOOST_TEST(columns.readRowCount() == initSize);
  columns.writeSegmentSeq(-1, table.nameCol, table.radecCol);
  BOOST_TEST(columns.readRowCount() == initSize * 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
