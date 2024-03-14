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
void check_slice_3d_is_read_back()
{
  Linx::Raster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Linx::Box<3> slice3d {{0, 0, 2}, {4, 5, 4}};
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(slice3d.shape());
  du.write(input(slice3d));
  const auto output = du.read<T, 3>();
  BOOST_TEST(output.shape() == slice3d.shape());
  for (const auto& p : output.domain()) {
    BOOST_TEST(output[p] == input[p + slice3d.front()]);
  }
}

template <>
void check_slice_3d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_slice_3d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define SLICE_3D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_3d_is_read_back_test) \
  { \
    check_slice_3d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(SLICE_3D_IS_READ_BACK_TEST)

template <typename T>
void check_slice_2d_is_read_back()
{
  Linx::Raster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Linx::Box<3> slice3d {{0, 2, 1}, {4, 5, 1}};
  const auto shape2d = Linx::slice<2>(slice3d.shape());
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(shape2d);
  du.write(input(slice3d));
  const auto output = du.read<T, 2>();
  BOOST_TEST(output.shape() == shape2d);
  for (const auto& p : output.domain()) {
    const auto q = Linx::extend<3>(p) + slice3d.front();
    BOOST_TEST(output[p] == input[q]);
  }
}

template <>
void check_slice_2d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_slice_2d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define SLICE_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_slice_2d_is_read_back_test) \
  { \
    check_slice_2d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(SLICE_2D_IS_READ_BACK_TEST)

template <typename T>
void check_region_2d_is_read_back()
{
  Linx::Raster<T, 3> input({5, 6, 7});
  for (auto p : input.domain()) {
    input[p] = 100 * p[2] + 10 * p[1] + p[0];
  }
  const Linx::Box<3> region3d {{0, 2, 1}, {4, 5, 1}};
  const auto shape2d = Linx::slice<2>(region3d.shape());
  Test::TemporarySifFile f;
  const auto& du = f.raster();
  du.update_type_shape<T>(shape2d);
  du.write(input(region3d));
  const auto output = du.read<T, 2>();
  BOOST_TEST(output.shape() == shape2d);
  for (const auto& p : output.domain()) {
    const auto q = Linx::extend<3>(p) + region3d.front();
    BOOST_TEST(output[p] == input[q]);
  }
}

template <>
void check_region_2d_is_read_back<char>()
{} // CFITSIO bug

template <>
void check_region_2d_is_read_back<std::uint64_t>()
{} // CFITSIO bug

#define REGION_2D_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_region_2d_is_read_back_test) \
  { \
    check_region_2d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(REGION_2D_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(const_data_raster_is_read_back_test, Test::TemporarySifFile)
{
  const Linx::Position<2> shape {7, 2};
  const auto c_data = Test::generate_random_vector<std::int16_t>(shape_size(shape)); // FIXME use Linx::random()
  const Linx::PtrRaster<const std::int16_t> c_raster(shape, c_data.data());
  this->write({}, c_raster);
  const auto res = this->raster().read<std::int16_t>();
  const auto& vec = res.container();
  BOOST_TEST(vec == c_data);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
