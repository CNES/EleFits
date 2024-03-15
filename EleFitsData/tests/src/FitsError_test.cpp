// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/FitsError.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsError_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(message_test)
{
  const std::string prefix = "EleFits";
  const std::string message = "MESSAGE!";
  FitsError error(message);
  const std::string output = error.what();
  const auto prefix_pos = output.find(prefix);
  BOOST_TEST(prefix_pos != std::string::npos);
  const auto message_pos = output.find(message, prefix_pos + prefix.length());
  BOOST_TEST(message_pos != std::string::npos);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
