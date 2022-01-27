/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFitsData//Region.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Region_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctors_test) {
  Position<7> front;
  ++front;
  Position<7> back {2, 8, 6, 1, 9, 8, 9};
  Region<7> fromTo {front, back};
  BOOST_TEST(fromTo.shape() == back);
  Region<7> fromShape {front, fromTo.shape()};
  BOOST_TEST(fromShape.back == back);
}

BOOST_AUTO_TEST_CASE(shift_test) {
  const Position<3> front {1, 2, 3};
  const Position<3> back {4, 5, 6};
  Region<3> region {front, back};
  const auto shape = region.shape();
  region += shape - 1;
  BOOST_TEST(region.shape() == shape);
  BOOST_TEST(region.front == back);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
