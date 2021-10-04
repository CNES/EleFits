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

#include "EL_FitsData/TestRaster.h"
#include "EL_FitsFile/FitsFileFixture.h"
#include "EL_FitsFile/ImageRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageRaster_test)

//-----------------------------------------------------------------------------

// Call graphs:
//
// readTo (raster)
//   read () => TEST
// readTo (subraster) => TEST
// readRegionTo (region, raster)
//   readRegion (region) => TEST
// readRegionTo (region, subraster) => TEST
//
// writeRegion (frontPosition, raster)
//   writeRegion (subraster) => TEST
// writeRegion (frontPosition, subraster) => TEST

template <typename T>
void checkRasterIsReadBack() {
  Test::RandomRaster<T, 3> input({ 16, 9, 3 });
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(input.shape);
  du.write(input);
  const auto output = du.read<T, 3>();
  BOOST_TEST(output.vector() == input.vector());
}

template <>
void checkRasterIsReadBack<char>() {
  // CFitsIO bug
}

template <>
void checkRasterIsReadBack<std::uint64_t>() {
  // CFitsIO bug
}

#define RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_raster_is_read_back_test) { \
    checkRasterIsReadBack<type>(); \
  }

template <typename T>
void checkRegionIsReadBack() {
  const Test::RandomRaster<T, 3> input({ 5, 6, 7 });
  const Region<3> region { { 1, 1, 1 }, { 2, 3, 4 } };
  const auto subinput = input.subraster(region);
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(input.shape);
  du.writeRegion(subinput);
  const auto output = du.readRegion<T, 3>(region);
  for (long z = 0; z < region.shape()[2]; ++z) {
    for (long y = 0; y < region.shape()[1]; ++y) {
      for (long x = 0; x < region.shape()[0]; ++x) {
        const auto o = output[{ x, y, z }];
        const auto i = input[{ x + region.front[0], y + region.front[1], z + region.front[2] }];
        BOOST_TEST(o == i);
      }
    }
  }
}

template <>
void checkRegionIsReadBack<char>() {
  // CFitsIO bug
}

template <>
void checkRegionIsReadBack<std::uint64_t>() {
  // CFitsIO bug
}

#define REGION_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_region_is_read_back_test) { \
    checkRegionIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(REGION_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(const_data_raster_is_read_back_test, Test::TemporarySifFile) {
  const Position<2> shape { 7, 2 };
  const auto cData = Test::generateRandomVector<std::int16_t>(shapeSize(shape));
  const PtrRaster<const std::int16_t> cRaster = makeRaster(shape, cData.data());
  writeRaster(cRaster);
  const auto res = readRaster<std::int16_t>();
  BOOST_TEST(res.vector() == cData);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
