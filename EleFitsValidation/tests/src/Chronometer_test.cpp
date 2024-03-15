// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/Chronometer.h"

#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <thread>

using namespace Fits;

struct ChronoFixture : public Validation::Chronometer<std::chrono::milliseconds> {
  ChronoFixture(std::chrono::milliseconds chrono_offset = std::chrono::milliseconds {std::rand()}) :
      Validation::Chronometer<std::chrono::milliseconds>(chrono_offset), offset(chrono_offset)
  {}
  void wait(std::int64_t ms = default_wait)
  {
    std::this_thread::sleep_for(Unit(ms));
  }
  Unit offset;
  static constexpr std::int64_t default_wait = 10;
};

constexpr std::int64_t ChronoFixture::default_wait;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Chronometer_test, ChronoFixture)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(init_test)
{
  BOOST_TEST(elapsed().count() == offset.count());
  BOOST_TEST(not is_running());
  BOOST_TEST(count() == 0);
}

BOOST_AUTO_TEST_CASE(one_inc_test)
{
  start();
  BOOST_TEST(is_running());
  wait();
  stop();
  BOOST_TEST(not is_running());
  BOOST_TEST(elapsed().count() >= offset.count());
  BOOST_TEST(count() == 1);
  const auto inc = last().count();
  BOOST_TEST(inc >= default_wait);
  BOOST_TEST(elapsed().count() == offset.count() + inc);
  BOOST_TEST(mean() == inc);
  BOOST_TEST(stdev() == 0.); // Exactly 0.
  BOOST_TEST(min() == inc);
  BOOST_TEST(max() == inc);
}

BOOST_AUTO_TEST_CASE(two_incs_test)
{
  start();
  wait(); // Wait
  stop();
  start();
  BOOST_TEST(is_running());
  wait(default_wait * 10); // Wait more
  stop();
  BOOST_TEST(not is_running());
  BOOST_TEST(elapsed().count() > offset.count());
  BOOST_TEST(count() == 2);
  const auto fast = increments()[0];
  const auto slow = increments()[1];
  BOOST_TEST(fast < slow); // FIXME Not that sure!
  BOOST_TEST(elapsed().count() == offset.count() + fast + slow);
  BOOST_TEST(mean() >= fast);
  BOOST_TEST(mean() <= slow);
  BOOST_TEST(stdev() > 0.);
  BOOST_TEST(min() == fast);
  BOOST_TEST(max() == slow);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
