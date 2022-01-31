// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/FitsError.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsError_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(message_test) {
  const std::string prefix = "EleFits";
  const std::string message = "MESSAGE!";
  FitsError error(message);
  const std::string output = error.what();
  const auto prefixPos = output.find(prefix);
  BOOST_TEST(prefixPos != std::string::npos);
  const auto messagePos = output.find(message, prefixPos + prefix.length());
  BOOST_TEST(messagePos != std::string::npos);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
