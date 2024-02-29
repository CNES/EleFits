// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/ImageRaster.h"
#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageRaster_test)

//-----------------------------------------------------------------------------

// Call graphs:
//
// read_to (raster)
//   read () => TEST
// read_to (subraster) => TEST
// read_region_to (region, raster)
//   read_region (region) => TEST
// read_regionTo (region, subraster) => TEST
//
// write_region (front_position, raster)
//   write_region (subraster) => TEST
// write_region (front_position, subraster) => TEST

template <typename T>
void check_raster_is_read_back()
{
  Test::RandomRaster<T, 3> input({16, 9, 3});
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(input.shape());
  du.write(input);
  const auto output = du.read<T, 3>();
  BOOST_TEST(output.container() == input.container());
}

template <>
void check_raster_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_raster_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_raster_is_read_back_test) \
  { \
    check_raster_is_read_back<type>(); \
  }

template <typename T>
void check_slice3d_is_read_back()
{
  VecRaster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> slice3d {{0, 0, 2}, {4, 5, 4}};
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(slice3d.shape());
  du.write_region(makeMemRegion(slice3d), input);
  const auto output = du.read<T, 3>();
  for (long z = 0; z < slice3d.shape()[2]; ++z) {
    for (long y = 0; y < slice3d.shape()[1]; ++y) {
      for (long x = 0; x < slice3d.shape()[0]; ++x) {
        const auto o = output[{x, y, z}];
        const auto i = input[{x + slice3d.front[0], y + slice3d.front[1], z + slice3d.front[2]}];
        BOOST_TEST(o == i);
      }
    }
  }
}

template <>
void check_slice3d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_slice3d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define SLICE_3D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_3d_is_read_back_test) \
  { \
    check_slice3d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(SLICE_3D_IS_READ_BACK_TEST)

template <typename T>
void check_slice2d_is_read_back()
{
  VecRaster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> slice2d {{0, 2, 1}, {4, 5, 1}};
  const Position<2> shape = slice2d.shape().slice<2>();
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(shape);
  du.write_region(makeMemRegion(slice2d), input);
  const auto output = du.read<T, 2>();
  for (long y = 0; y < shape[1]; ++y) {
    for (long x = 0; x < shape[0]; ++x) {
      const auto o = output[{x, y}];
      const auto i = input[{x + slice2d.front[0], y + slice2d.front[1], slice2d.front[2]}];
      BOOST_TEST(o == i);
    }
  }
}

template <>
void check_slice2d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_slice2d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define SLICE_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_2d_is_read_back_test) \
  { \
    check_slice2d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(SLICE_2D_IS_READ_BACK_TEST)

template <typename T>
void check_subraster2d_is_read_back()
{
  VecRaster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Region<3> region2d {{1, 2, 1}, {3, 5, 1}};
  const Position<2> shape = region2d.shape().slice<2>();
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(shape);
  du.write_region(makeMemRegion(region2d), input);
  const auto output = du.read<T, 2>();
  for (long y = 0; y < shape[1]; ++y) {
    for (long x = 0; x < shape[0]; ++x) {
      const auto o = output[{x, y}];
      const auto i = input[{x + region2d.front[0], y + region2d.front[1], region2d.front[2]}];
      BOOST_TEST(o == i);
    }
  }
}

template <>
void check_subraster2d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_subraster2d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define SUBRASTER_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_subraster_2d_is_read_back_test) \
  { \
    check_subraster2d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(SUBRASTER_2D_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(const_data_raster_is_read_back_test, Test::TemporarySifFile)
{
  const Position<2> shape {7, 2};
  const auto c_data = Test::generate_random_vector<std::int16_t>(shape_size(shape));
  const PtrRaster<const std::int16_t> c_raster(shape, c_data.data());
  this->write({}, c_raster);
  const auto res = this->raster().read<std::int16_t>();
  const auto& vec = res.container();
  BOOST_TEST(vec == c_data);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
