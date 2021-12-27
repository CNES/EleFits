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

#include "EleFitsData/Raster.h"
#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Subraster_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(singleton_subraster_test) {
  const Position<3> shape {3, 4, 5};
  const Test::RandomRaster<float, 3> raster(shape);
  const Position<3> pos {1, 2, 3};
  const Region<3> region {pos, pos};
  const Subraster<float, 3, std::vector<float>> subraster(raster, region);
  BOOST_TEST(subraster.size() == 1);
  BOOST_TEST((subraster[{0, 0, 0}] == raster[pos]));
}

BOOST_AUTO_TEST_CASE(domain_subraster_test) {
  const Position<3> shape {3, 4, 5};
  const Test::RandomRaster<float, 3> raster(shape);
  const auto region = raster.domain();
  const Subraster<float, 3, std::vector<float>> subraster(raster, region);
  BOOST_TEST(subraster.shape() == shape);
  for (Position<3> p : region) {
    BOOST_TEST(subraster[p] == raster[p]);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
