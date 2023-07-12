// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/CompressionStrategy.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CompressionStrategy_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(lossless_ctor_test) {
  auto strategy = BasicCompressionStrategy::lossless();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
