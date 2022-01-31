// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/CfitsioBenchmark.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioBenchmark_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(parameter_test) {

  Validation::BenchmarkFactory factory;
  factory.registerBenchmark<Validation::CfitsioBenchmark>("colwise", -1);
  factory.registerBenchmark<Validation::CfitsioBenchmark>("optimal", 0);
  factory.registerBenchmark<Validation::CfitsioBenchmark>("rowwise", 1);

  // Store a reference to unique_ptr to avoid corresponding constructor to be called
  const auto colwise = factory.createBenchmark("colwise", "file.fits");
  const auto optimal = factory.createBenchmark("optimal", "file.fits");
  const auto rowwise = factory.createBenchmark("rowwise", "file.fits");

  // Cast to raw pointers
  const auto colwiseCPtr = dynamic_cast<Validation::CfitsioBenchmark*>(colwise.get());
  const auto optimalPtr = dynamic_cast<Validation::CfitsioBenchmark*>(optimal.get());
  const auto rowwisePtr = dynamic_cast<Validation::CfitsioBenchmark*>(rowwise.get());

  BOOST_TEST(colwiseCPtr->rowChunkSize() == -1);
  BOOST_TEST(optimalPtr->rowChunkSize() == 0);
  BOOST_TEST(rowwisePtr->rowChunkSize() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
