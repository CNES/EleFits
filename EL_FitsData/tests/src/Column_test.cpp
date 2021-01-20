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

#include <boost/test/unit_test.hpp>

#include "EL_FitsData/Column.h"
#include "EL_FitsData/TestColumn.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Column_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(column_data_can_be_shared_test) {
  std::vector<int> input { 1, 2, 3 };
  VecRefColumn<int> column({ "SHARED", "", 1 }, input);
  BOOST_CHECK_EQUAL(column.data()[1], 2);
  input[1] = 4;
  BOOST_CHECK_EQUAL(column.data()[1], 4);
}

BOOST_AUTO_TEST_CASE(column_data_can_be_moved_test) {
  std::vector<int> input { 4, 5, 6 };
  VecColumn<int> column({ "DATA", "", 1 }, std::move(input));
  BOOST_CHECK_EQUAL(column.vector()[1], 5);
  BOOST_CHECK_EQUAL(input.size(), 0);
  input = std::move(column.vector());
  BOOST_CHECK_EQUAL(input[1], 5);
  BOOST_CHECK_EQUAL(column.vector().size(), 0);
  BOOST_CHECK_EQUAL(column.elementCount(), 0);
}

BOOST_AUTO_TEST_CASE(subscript_bounds_test) {
  const long rowCount = 10;
  const long repeatCount = 3;
  Test::RandomVectorColumn<int> column(repeatCount, rowCount);
  column.at(1, -1) = 1;
  BOOST_CHECK_EQUAL(column.at(1, -1), 1);
  const auto &vec = column.vector();
  BOOST_CHECK_EQUAL(column.at(0), vec[0]);
  BOOST_CHECK_EQUAL(column.at(-1), vec[(rowCount - 1) * repeatCount]);
  BOOST_CHECK_EQUAL(column.at(-rowCount), vec[0]);
  BOOST_CHECK_EQUAL(column.at(0, -1), vec[repeatCount - 1]);
  BOOST_CHECK_EQUAL(column.at(-1, -1), vec[rowCount * repeatCount - 1]);
  BOOST_CHECK_THROW(column.at(rowCount), FitsIOError);
  BOOST_CHECK_THROW(column.at(-1 - rowCount), FitsIOError);
  BOOST_CHECK_THROW(column.at(0, repeatCount), FitsIOError);
  BOOST_CHECK_THROW(column.at(0, -1 - repeatCount), FitsIOError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
