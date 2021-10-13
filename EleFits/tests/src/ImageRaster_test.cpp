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

#include "EleFitsData/TestRaster.h"
#include "EleFits/FitsFileFixture.h"
#include "EleFits/ImageRaster.h"

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
void checkRasterIsReadBack<char>() {} // CFitsIO bug

template <>
void checkRasterIsReadBack<std::uint64_t>() {} // CFitsIO bug

#define RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_raster_is_read_back_test) { \
    checkRasterIsReadBack<type>(); \
  }

template <typename T>
void checkSlice3DIsReadBack() {
  VecRaster<T, 3> input({ 5, 6, 7 });
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> slice3D { { 0, 0, 2 }, { 4, 5, 4 } };
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(slice3D.shape());
  du.writeRegion(makeMemRegion(slice3D), input);
  const auto output = du.read<T, 3>();
  for (long z = 0; z < slice3D.shape()[2]; ++z) {
    for (long y = 0; y < slice3D.shape()[1]; ++y) {
      for (long x = 0; x < slice3D.shape()[0]; ++x) {
        const auto o = output[{ x, y, z }];
        const auto i = input[{ x + slice3D.front[0], y + slice3D.front[1], z + slice3D.front[2] }];
        BOOST_TEST(o == i);
      }
    }
  }
}

template <>
void checkSlice3DIsReadBack<char>() {} // CFitsIO bug

template <>
void checkSlice3DIsReadBack<std::uint64_t>() {} // CFitsIO bug

#define SLICE_3D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_3d_is_read_back_test) { \
    checkSlice3DIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(SLICE_3D_IS_READ_BACK_TEST)

template <typename T>
void checkSlice2DIsReadBack() {
  VecRaster<T, 3> input({ 5, 6, 7 });
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> slice2D { { 0, 2, 1 }, { 4, 5, 1 } };
  const Position<2> shape = slice2D.shape().slice<2>();
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(shape);
  du.writeRegion(makeMemRegion(slice2D), input);
  const auto output = du.read<T, 2>();
  for (long y = 0; y < shape[1]; ++y) {
    for (long x = 0; x < shape[0]; ++x) {
      const auto o = output[{ x, y }];
      const auto i = input[{ x + slice2D.front[0], y + slice2D.front[1], slice2D.front[2] }];
      BOOST_TEST(o == i);
    }
  }
}

template <>
void checkSlice2DIsReadBack<char>() {} // CFitsIO bug

template <>
void checkSlice2DIsReadBack<std::uint64_t>() {} // CFitsIO bug

#define SLICE_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_2d_is_read_back_test) { \
    checkSlice2DIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(SLICE_2D_IS_READ_BACK_TEST)

template <typename T>
void checkSubraster2DIsReadBack() {
  VecRaster<T, 3> input({ 5, 6, 7 });
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> region2D { { 1, 2, 1 }, { 3, 5, 1 } };
  const Position<2> shape = region2D.shape().slice<2>();
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.reinit<T>(shape);
  du.writeRegion(makeMemRegion(region2D), input);
  const auto output = du.read<T, 2>();
  for (long y = 0; y < shape[1]; ++y) {
    for (long x = 0; x < shape[0]; ++x) {
      const auto o = output[{ x, y }];
      const auto i = input[{ x + region2D.front[0], y + region2D.front[1], region2D.front[2] }];
      BOOST_TEST(o == i);
    }
  }
}

template <>
void checkSubraster2DIsReadBack<char>() {} // CFitsIO bug

template <>
void checkSubraster2DIsReadBack<std::uint64_t>() {} // CFitsIO bug

#define SUBRASTER_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_subraster_2d_is_read_back_test) { \
    checkSubraster2DIsReadBack<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(SUBRASTER_2D_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(const_data_raster_is_read_back_test, Test::TemporarySifFile) {
  const Position<2> shape { 7, 2 };
  const auto cData = Test::generateRandomVector<std::int16_t>(shapeSize(shape));
  const PtrRaster<const std::int16_t> cRaster(shape, cData.data());
  writeRaster(cRaster);
  const auto res = readRaster<std::int16_t>();
  BOOST_TEST(res.vector() == cData);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
