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

#include "EL_FitsFile/BintableColumns.h"

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE(example_test) {

  BOOST_FAIL("!!!! Please implement your tests !!!!");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
