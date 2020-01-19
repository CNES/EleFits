/**
 * @file tests/src/Record_test.cpp
 * @date 09/03/19
 * @author user
 *
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

#include "EL_FitsData//Record.h"

#include "FixtureRandom.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (Record_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( full_init_test ) {

  Record<int> full { "FULL", 4, "m", "Full" };

  BOOST_CHECK_EQUAL(full.keyword, "FULL");
  BOOST_CHECK_EQUAL(full.value, 4);
  BOOST_CHECK_EQUAL(full.unit, "m");
  BOOST_CHECK_EQUAL(full.comment, "Full");

}

BOOST_AUTO_TEST_CASE( unit_init_test ) {

  Record<int> unit { "UNIT", 3, "m" };

  BOOST_CHECK_EQUAL(unit.keyword, "UNIT");
  BOOST_CHECK_EQUAL(unit.value, 3);
  BOOST_CHECK_EQUAL(unit.unit, "m");
  BOOST_CHECK_EQUAL(unit.comment, "");
  
}

BOOST_AUTO_TEST_CASE( mini_init_test ) {

  Record<int> mini { "MINI", 2 };

  BOOST_CHECK_EQUAL(mini.keyword, "MINI");
  BOOST_CHECK_EQUAL(mini.value, 2);
  BOOST_CHECK_EQUAL(mini.unit, "");
  BOOST_CHECK_EQUAL(mini.comment, "");
  
}

template<typename T>
void check_equal(T value, T expected) {
  BOOST_CHECK_EQUAL(value, expected);
}

template<typename T>
void check_cast() {
  T v = Test::generate_random_value<T>();
  Record<T> r("KEY", v);
  check_equal<T>(r, v);
}

#define TEST_CAST_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE( name##_test ) { check_cast<type>(); }

#define TEST_CAST(type) \
  TEST_CAST_ALIAS(type, type)

#define TEST_CAST_UNSIGNED(type) \
  TEST_CAST_ALIAS(unsigned type, u##type)

TEST_CAST(bool)
TEST_CAST(int)
TEST_CAST_ALIAS(std::string, string)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


