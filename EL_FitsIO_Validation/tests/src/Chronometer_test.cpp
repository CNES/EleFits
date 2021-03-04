/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <thread>

#include "EL_FitsIO_Validation/Chronometer.h"

using namespace Euclid::FitsIO;

struct ChronoFixture : public Test::Chronometer<std::chrono::milliseconds> {
  ChronoFixture(std::chrono::milliseconds chronoOffset = std::chrono::milliseconds { std::rand() }) :
      Test::Chronometer<std::chrono::milliseconds>(chronoOffset),
      offset(chronoOffset) {
  }
  void wait(std::int64_t duration = 2) {
    std::this_thread::sleep_for(Unit(duration));
  }
  Unit offset;
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Chronometer_test, ChronoFixture)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(init_test) {
  BOOST_CHECK_EQUAL(elapsed().count(), offset.count());
  BOOST_CHECK(not isRunning());
  BOOST_CHECK_EQUAL(count(), 0);
}

BOOST_AUTO_TEST_CASE(one_inc_test) {
  start();
  BOOST_CHECK(isRunning());
  wait();
  stop();
  BOOST_CHECK(not isRunning());
  BOOST_CHECK_GT(elapsed().count(), offset.count());
  BOOST_CHECK_EQUAL(count(), 1);
  const auto inc = last().count();
  BOOST_CHECK_EQUAL(elapsed().count(), offset.count() + inc);
  BOOST_CHECK_EQUAL(mean(), inc);
  BOOST_CHECK_EQUAL(stdev(), 0.);
  BOOST_CHECK_EQUAL(min(), inc);
  BOOST_CHECK_EQUAL(max(), inc);
}

BOOST_AUTO_TEST_CASE(two_incs_test) {
  start();
  wait();
  stop();
  start();
  BOOST_CHECK(isRunning());
  wait();
  wait(); // Wait more
  stop();
  BOOST_CHECK(not isRunning());
  BOOST_CHECK_GT(elapsed().count(), offset.count());
  BOOST_CHECK_EQUAL(count(), 2);
  const auto first = increments()[0];
  const auto second = increments()[1];
  BOOST_CHECK_EQUAL(elapsed().count(), offset.count() + first + second);
  BOOST_CHECK_GE(mean(), first);
  BOOST_CHECK_LE(mean(), second);
  BOOST_CHECK_GT(stdev(), 0.);
  BOOST_CHECK_EQUAL(min(), first);
  BOOST_CHECK_EQUAL(max(), second);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
