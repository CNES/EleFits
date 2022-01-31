// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData//Region.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Region_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctors_test) {
  Position<7> front;
  ++front;
  Position<7> back {2, 8, 6, 1, 9, 8, 9};
  Region<7> fromTo {front, back};
  BOOST_TEST(fromTo.shape() == back);
  Region<7> fromShape {front, fromTo.shape()};
  BOOST_TEST(fromShape.back == back);
}

BOOST_AUTO_TEST_CASE(shift_test) {
  const Position<3> front {1, 2, 3};
  const Position<3> back {4, 5, 6};
  Region<3> region {front, back};
  const auto shape = region.shape();
  region += shape - 1;
  BOOST_TEST(region.shape() == shape);
  BOOST_TEST(region.front == back);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
