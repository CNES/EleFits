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

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkRandom3DRasterIsReadBack() {
  using namespace FitsIO::Test;
  RandomRaster<T, 3> input({ 2, 3, 4 });
  MinimalFile file;
  HduAccess::createImageExtension(file.fptr, "IMGEXT", input);
  const auto fixedOutput = ImageIo::readRaster<T, 3>(file.fptr);
  BOOST_TEST(fixedOutput.vector() == input.vector());
  const auto variableOuptut = ImageIo::readRaster<T, -1>(file.fptr);
  BOOST_TEST(variableOuptut.dimension() == 3);
  BOOST_TEST(variableOuptut.vector() == input.vector());
}

#define RANDOM_3D_RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_3d_raster_is_read_back_test) { \
    checkRandom3DRasterIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(RANDOM_3D_RASTER_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(region_is_read_back, FitsIO::Test::MinimalFile) {
  FitsIO::VecRaster<long, 3> input({ 3, 4, 5 });
  for (long z = 0; z < input.length<2>(); ++z) {
    for (long y = 0; y < input.length<1>(); ++y) {
      for (long x = 0; x < input.length<0>(); ++x) {
        input[{ x, y, z }] = x * 100 + y * 10 + z;
      }
    }
  }
  HduAccess::createImageExtension(fptr, "EXT", input);
  const auto region = FitsIO::Region<3>::fromShape({ 1, 0, 1 }, { 2, 3, 3 });
  const auto view = ImageIo::readRegion<long, 3>(fptr, region);
  BOOST_TEST(view.shape == region.shape());
  for (long z = 0; z < view.length<2>(); ++z) {
    for (long y = 0; y < view.length<1>(); ++y) {
      for (long x = 0; x < view.length<0>(); ++x) {
        const auto& v = view[{ x, y, z }];
        const auto& i = input[{ x + 1, y + 0, z + 1 }]; // TODO no hardcoded offsets
        BOOST_TEST(v == i);
      }
    }
  }
  FitsIO::VecRaster<long, 3> output({ 3, 4, 5 });
  FitsIO::Subraster<long, 3> dst { output, region }; // TODO don't use the same region
  ImageIo::readRegionTo<long, 3>(fptr, region, dst);
  for (long z = region.front[2]; z <= region.back[2]; ++z) {
    for (long y = region.front[1]; y <= region.back[1]; ++y) {
      for (long x = region.front[0]; x <= region.back[0]; ++x) {
        const auto& o = output[{ x, y, z }];
        const auto& i = input[{ x, y, z }];
        BOOST_TEST(o == i);
      }
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
