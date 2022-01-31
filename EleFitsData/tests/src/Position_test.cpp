// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Position.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Position_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(vector_init_test) {

  const std::vector<long> indices {1, 2, 3};

  Position<-1> braceList {indices[0], indices[1], indices[2]};
  BOOST_TEST(braceList.size() == indices.size());

  Position<-1> iterators(indices.begin(), indices.end());
  BOOST_TEST(iterators.size() == indices.size());

  Position<-1> dimension(indices.size());
  for (std::size_t i = 0; i < indices.size(); ++i) {
    dimension[i] = indices[i];
  }

  for (std::size_t i = 0; i < indices.size(); ++i) {
    BOOST_TEST(braceList[i] == indices[i]);
    BOOST_TEST(iterators[i] == indices[i]);
    BOOST_TEST(dimension[i] == indices[i]);
  }
}

BOOST_AUTO_TEST_CASE(array_init_test) {

  const std::array<long, 3> indices {1, 2, 3};

  Position<3> braceList {indices[0], indices[1], indices[2]};
  BOOST_TEST(braceList.size() == indices.size());

  Position<3> iterators(indices.begin(), indices.end());
  BOOST_TEST(iterators.size() == indices.size());

  Position<3> dimension(indices.size());
  for (std::size_t i = 0; i < indices.size(); ++i) {
    dimension[i] = indices[i];
  }

  for (std::size_t i = 0; i < indices.size(); ++i) {
    BOOST_TEST(braceList[i] == indices[i]);
    BOOST_TEST(iterators[i] == indices[i]);
    BOOST_TEST(dimension[i] == indices[i]);
  }
}

BOOST_AUTO_TEST_CASE(arithmetics_test) {

  const Position<4> indices {0, 1, 2, 3};
  const Position<4> evens {0, 2, 4, 6};
  const Position<4> positives {1, 2, 3, 4};

  const auto plus = indices + indices;
  BOOST_TEST(plus == evens);
  const auto minus = plus - indices;
  BOOST_TEST(minus == indices);

  const auto multiplies = indices * 2;
  BOOST_TEST(multiplies == evens);
  const auto divides = multiplies / 2;
  BOOST_TEST(divides == indices);

  const auto inc = indices + 1;
  BOOST_TEST(inc == positives);
  const auto dec = inc - 1;
  BOOST_TEST(dec == indices);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
