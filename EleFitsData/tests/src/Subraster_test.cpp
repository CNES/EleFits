// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
