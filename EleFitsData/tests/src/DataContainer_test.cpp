// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/DataContainer.h"
#include "EleFitsData/Position.h"

#include <boost/test/unit_test.hpp>
#include <sstream>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(DataContainer_test)

//-----------------------------------------------------------------------------

template <long M, long N>
void checkStreamInsertion(const Position<N>& position, const std::string& expected) {
  const auto slice = position.template slice<M>();
  std::stringstream os;
  os << slice;
  BOOST_TEST(os.str() == expected);
}

BOOST_AUTO_TEST_CASE(stream_insertion_test) {
  Position<10> a {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  checkStreamInsertion<0>(a, "[]");
  checkStreamInsertion<1>(a, "[0]");
  checkStreamInsertion<2>(a, "[0, 1]");
  checkStreamInsertion<3>(a, "[0, 1, 2]");
  checkStreamInsertion<4>(a, "[0, 1, 2, 3]");
  checkStreamInsertion<5>(a, "[0, 1, 2, 3, 4]");
  checkStreamInsertion<6>(a, "[0, 1, 2, 3, 4, 5]");
  checkStreamInsertion<7>(a, "[0, 1, 2, 3, 4, 5, 6]");
  checkStreamInsertion<8>(a, "[0, 1, 2 ... 5, 6, 7]");
  checkStreamInsertion<9>(a, "[0, 1, 2 ... 6, 7, 8]");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
