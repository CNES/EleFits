// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
  std::cout << "random ND" << std::endl;
  using namespace Fits::Test;
  RandomRaster<T, -1> input({2, 3, 4});
  MinimalFile file;
  std::cout << "assign_image()" << std::endl;
  HduAccess::assign_image(file.fptr, "IMGEXT", input);
  std::cout << "read_shape()" << std::endl;
  const auto variable_shape = ImageIo::read_shape<-1>(file.fptr);
  std::cout << variable_shape << std::endl;
  BOOST_TEST(variable_shape == input.shape());
  std::cout << "read_raster()" << std::endl;
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

BOOST_FIXTURE_TEST_CASE(region_is_read_back, Fits::Test::MinimalFile)
{
  Linx::Raster<long, 3> input({3, 4, 5});
  for (long z = 0; z < input.length(2); ++z) {
    for (long y = 0; y < input.length(1); ++y) {
      for (long x = 0; x < input.length(0); ++x) {
        input[{x, y, z}] = x * 100 + y * 10 + z;
      }
    }
  }
  HduAccess::assign_image(fptr, "EXT", input);
  const auto region = Linx::Box<3>::from_shape({1, 0, 1}, {2, 3, 3});
  const auto view = ImageIo::read_region<long, 3>(fptr, region);
  BOOST_TEST(view.shape() == region.shape());
  for (long z = 0; z < view.length(2); ++z) {
    for (long y = 0; y < view.length(1); ++y) {
      for (long x = 0; x < view.length(0); ++x) {
        const auto& v = view[{x, y, z}];
        const auto& i = input[{x + 1, y + 0, z + 1}]; // TODO no hardcoded offsets
        BOOST_TEST(v == i);
      }
    }
  }
  Linx::Raster<long, 3> output({3, 4, 5});
  Linx::Raster<long, 3>::Tile<3> dst {output, region}; // TODO don't use the same region
  ImageIo::read_region_to(fptr, region, dst);
  for (long z = region.front()[2]; z <= region.back()[2]; ++z) {
    for (long y = region.front()[1]; y <= region.back()[1]; ++y) {
      for (long x = region.front()[0]; x <= region.back()[0]; ++x) {
        const auto& o = output[{x, y, z}];
        const auto& i = input[{x, y, z}];
        BOOST_TEST(o == i);
      }
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
