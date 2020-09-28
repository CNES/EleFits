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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsData/Raster.h"

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Raster_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(index_test) {

  FitsIO::Position<4> shape;
  for (auto &length : shape) {
    length = std::rand();
  }
  FitsIO::Position<4> pos;
  for (auto &coord : pos) {
    coord = std::rand();
  }
  auto index = FitsIO::Internal::IndexImpl<3>::template offset<4>(shape, pos);
  BOOST_CHECK_EQUAL(index, pos[0] + shape[0] * (pos[1] + shape[1] * (pos[2] + shape[2] * (pos[3]))));
}

BOOST_FIXTURE_TEST_CASE(raster_2D_test, FitsIO::Test::SmallRaster) {

  long size(this->width * this->height);
  BOOST_CHECK_EQUAL(this->size(), size);
  BOOST_CHECK_EQUAL(this->vector().size(), size);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
