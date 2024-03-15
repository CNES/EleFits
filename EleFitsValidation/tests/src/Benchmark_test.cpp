// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/Benchmark.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

struct ParamBenchmark : Validation::Benchmark {
  ParamBenchmark(const std::string& filename, int i, double d) : Validation::Benchmark(filename), m_i(i), m_d(d) {}
  void open() {};
  void close() {};
  int m_i;
  double m_d;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Benchmark_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(factory_test)
{
  Validation::BenchmarkFactory factory;
  factory.register_benchmark<ParamBenchmark>("0, 3.14", 0, 3.14);
  factory.register_benchmark<ParamBenchmark>("-1, 0.", -1, 0.);

  const auto b0 = factory.create_benchmark("0, 3.14", "file.fits");
  const auto* pb0 = dynamic_cast<const ParamBenchmark*>(b0.get());
  BOOST_TEST(pb0->m_i == 0);
  BOOST_TEST(pb0->m_d == 3.14);

  const auto b1 = factory.create_benchmark("-1, 0.", "file.fits");
  const auto* pb1 = dynamic_cast<const ParamBenchmark*>(b1.get());
  BOOST_TEST(pb1->m_i == -1);
  BOOST_TEST(pb1->m_d == 0.);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
