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

#include "EleFitsData/PositionIterator.h"
#include "EleFitsData/Raster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PositionIterator_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(shape_is_screened_in_order_test) {
  Position<5> shape { 2, 3, 4, 5, 6 };
  VecRaster<long, 5> raster(shape);
  for (long i = 0; i < raster.size(); ++i) {
    *(raster.data() + i) = i; // FIXME implement Raster::operator[](long)
  }
  long i = 0;
  for (const auto& p : raster.domain()) {
    BOOST_TEST(raster.index(p) == i);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(region_is_screened_in_order_test) {
  Position<4> shape { 3, 4, 5, 6 };
  VecRaster<long, 4> raster(shape);
  Region<4> region { Position<4>::zero() + 1, shape - 2 };
  for (long i = 0; i < raster.size(); ++i) {
    *(raster.data() + i) = i; // FIXME implement Raster::operator[](long)
  }
  long current = 0;
  long count = 0;
  for (const auto& p : region) {
    BOOST_TEST(raster.index(p) > current);
    current = raster.index(p);
    ++count;
  }
  BOOST_TEST(count == region.size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
