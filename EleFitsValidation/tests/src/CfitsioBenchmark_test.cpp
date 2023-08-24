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
  factory.register_benchmark<Validation::CfitsioBenchmark>("colwise", -1);
  factory.register_benchmark<Validation::CfitsioBenchmark>("optimal", 0);
  factory.register_benchmark<Validation::CfitsioBenchmark>("rowwise", 1);

  // Store a reference to unique_ptr to avoid corresponding constructor to be called
  const auto colwise = factory.create_benchmark("colwise", "file.fits");
  const auto optimal = factory.create_benchmark("optimal", "file.fits");
  const auto rowwise = factory.create_benchmark("rowwise", "file.fits");

  // Cast to raw pointers
  const auto colwise_c_ptr = dynamic_cast<Validation::CfitsioBenchmark*>(colwise.get());
  const auto optimal_ptr = dynamic_cast<Validation::CfitsioBenchmark*>(optimal.get());
  const auto rowwise_ptr = dynamic_cast<Validation::CfitsioBenchmark*>(rowwise.get());

  BOOST_TEST(colwise_c_ptr->chunk_row_count() == -1);
  BOOST_TEST(optimal_ptr->chunk_row_count() == 0);
  BOOST_TEST(rowwise_ptr->chunk_row_count() == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
