/**
 * @file tests/src/Column_test.cpp
 * @date 10/21/19
 * @author user
 *
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

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Column_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( share_test ) {
  std::vector<int> input { 1, 2, 3 };
  FitsIO::VecRefColumn<int> column({"SHARED", "", 1}, input);
  BOOST_CHECK_EQUAL(column.data()[1], 2);
  input[1] = 4;
  BOOST_CHECK_EQUAL(column.data()[1], 4);
}

BOOST_AUTO_TEST_CASE( move_test ) {
  std::vector<int> input { 4, 5, 6 };
  FitsIO::VecColumn<int> column({"DATA", "", 1}, std::move(input));
  BOOST_CHECK_EQUAL(column.vector()[1], 5);
  BOOST_CHECK_EQUAL(input.size(), 0);
  input = std::move(column.vector());
  BOOST_CHECK_EQUAL(input[1], 5);
  BOOST_CHECK_EQUAL(column.vector().size(), 0);
  BOOST_CHECK_EQUAL(column.rows(), 0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
