// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void check_random_3d_is_read_back()
{
  using namespace Fits::Test;
  RandomRaster<T, 3> input({2, 3, 4});
  MinimalFile file;
  HduAccess::assign_image(file.fptr, "IMGEXT", input);
  const auto fixed_shape = ImageIo::read_shape<3>(file.fptr);
  BOOST_TEST(fixed_shape == input.shape());
  const auto fixed_output = ImageIo::read_raster<T, 3>(file.fptr);
  BOOST_TEST(fixed_output.container() == input.container());
}

template <typename T>
void check_random_nd_is_read_back()
{
  using namespace Fits::Test;
  RandomRaster<T, -1> input({2, 3, 4});
  MinimalFile file;
  HduAccess::assign_image(file.fptr, "IMGEXT", input);
  const auto variable_shape = ImageIo::read_shape<-1>(file.fptr);
  BOOST_TEST(variable_shape == input.shape());
  const auto variable_output = ImageIo::read_raster<T, -1>(file.fptr);
  BOOST_TEST(variable_output.dimension() == 3);
  BOOST_TEST(variable_output.container() == input.container());
}

#define RANDOM_3D_RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_3d_raster_is_read_back_test) \
  { \
    check_random_3d_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_3D_RASTER_IS_READ_BACK_TEST)

#define RANDOM_ND_RASTER_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_nd_raster_is_read_back_test) \
  { \
    check_random_nd_is_read_back<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_ND_RASTER_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(region_is_read_back_test, Fits::Test::MinimalFile)
{
  Linx::Raster<long, 3> input({3, 4, 5});
  input.generate(
      [](const auto& p) {
        return p[0] * 100 + p[1] * 10 + p[2];
      },
      input.domain());
  HduAccess::assign_image(fptr, "EXT", input);
  const auto region = Linx::Box<3>::from_shape({1, 0, 1}, {2, 3, 3});

  const auto view = ImageIo::read_region<long, 3>(fptr, region);
  BOOST_TEST(view.shape() == region.shape());
  for (const auto& p : view.domain()) {
    const auto& v = view[p];
    const auto& i = input[p + region.front()];
    BOOST_TEST(v == i);
  }

  Linx::Raster<long, 3> output(input.shape());
  auto dst = output(region); // TODO don't use the same region
  ImageIo::read_region_to(fptr, region, dst);
  for (const auto& p : region) {
    const auto& o = output[p];
    const auto& i = input[p];
    BOOST_TEST(o == i);
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
