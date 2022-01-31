// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/PositionIterator.h"
#include "EleFitsData/Raster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PositionIterator_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(shape_is_screened_in_order_test) {
  Position<5> shape {2, 3, 4, 5, 6};
  VecRaster<long, 5> raster(shape);
  for (std::size_t i = 0; i < raster.size(); ++i) {
    raster[i] = i;
  }
  long i = 0;
  for (const auto& p : raster.domain()) {
    BOOST_TEST(raster.index(p) == i);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(region_is_screened_in_order_test) {
  Position<4> shape {3, 4, 5, 6};
  VecRaster<long, 4> raster(shape);
  Region<4> region {Position<4>::zero() + 1, shape - 2};
  for (std::size_t i = 0; i < raster.size(); ++i) {
    raster[i] = i;
  }
  long current = 0;
  long count = 0;
  for (const auto& p : region) {
    BOOST_TEST(raster.index(p) > current);
    current = raster.index(p);
    ++count;
  }
  BOOST_TEST(count == region.size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
