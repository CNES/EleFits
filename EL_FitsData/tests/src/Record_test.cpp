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

#include "EL_FitsData/TestUtils.h"
#include "EL_FitsData/Record.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Record_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(full_init_test) {

  Record<int> full { "FULL", 4, "m", "Full" };

  BOOST_CHECK_EQUAL(full.keyword, "FULL");
  BOOST_CHECK_EQUAL(full.value, 4);
  BOOST_CHECK_EQUAL(full.unit, "m");
  BOOST_CHECK_EQUAL(full.comment, "Full");
}

BOOST_AUTO_TEST_CASE(unit_init_test) {

  Record<int> unit { "UNIT", 3, "m" };

  BOOST_CHECK_EQUAL(unit.keyword, "UNIT");
  BOOST_CHECK_EQUAL(unit.value, 3);
  BOOST_CHECK_EQUAL(unit.unit, "m");
  BOOST_CHECK_EQUAL(unit.comment, "");
}

BOOST_AUTO_TEST_CASE(mini_init_test) {

  Record<int> mini { "MINI", 2 };

  BOOST_CHECK_EQUAL(mini.keyword, "MINI");
  BOOST_CHECK_EQUAL(mini.value, 2);
  BOOST_CHECK_EQUAL(mini.unit, "");
  BOOST_CHECK_EQUAL(mini.comment, "");
}

BOOST_AUTO_TEST_CASE(rawComment_test) {
  Record<int> noUnit { "V", 1, "", "Speed" };
  Record<int> unit { "V", 1, "m", "Speed" };
  Record<int> unitInComment { "V", 1, "", "[m] Speed" };
  BOOST_CHECK_EQUAL(noUnit.rawComment(), "Speed");
  BOOST_CHECK_EQUAL(unit.rawComment(), "[m] Speed");
  BOOST_CHECK_EQUAL(unitInComment.rawComment(), "[m] Speed");
}

template <typename T>
void checkEqual(T value, T expected) {
  BOOST_CHECK_EQUAL(value, expected);
}

template <typename T>
void checkCast() {
  T v = Test::generateRandomValue<T>();
  Record<T> r("KEY", v);
  checkEqual<T>(r, v);
}

#define TEST_CAST_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE(name##_test) { \
    checkCast<type>(); \
  }

#define TEST_CAST(type) TEST_CAST_ALIAS(type, type)

#define TEST_CAST_UNSIGNED(type) TEST_CAST_ALIAS(unsigned type, u##type)

TEST_CAST(bool)
TEST_CAST(int)
TEST_CAST_ALIAS(std::string, string)

template <typename TV, typename TE>
void checkApprox(TV value, TE expected) {
  BOOST_CHECK_LE(std::abs(static_cast<TV>(expected) - value), std::numeric_limits<TV>::epsilon() * 2);
}

template <typename TV, typename TE>
void checkApprox(std::complex<TV> value, std::complex<TE> expected) {
  checkApprox(value.real(), expected.real());
  checkApprox(value.imag(), expected.imag());
}

template <>
void checkApprox(std::string value, std::string expected) {
  BOOST_CHECK_EQUAL(value, expected);
}

template <typename TFrom, typename TTo>
void checkCaster() {
  TFrom input = Test::generateRandomValue<TFrom>();
  TTo output = Record<TTo>::cast(input);
  checkApprox(output, input);
}

template <typename TFrom, typename TTo>
void checkCasterFromAny() {
  TFrom value = Test::generateRandomValue<TFrom>();
  boost::any input(value);
  TTo output = Record<TTo>::cast(input);
  checkApprox(output, value);
}

BOOST_AUTO_TEST_CASE(cast_scalar_test) {
  checkCaster<bool, long long>();
  checkCaster<char, long>();
  checkCaster<short, int>();
  checkCaster<float, double>();
}

BOOST_AUTO_TEST_CASE(cast_complex_test) {
  checkCaster<std::complex<float>, std::complex<float>>();
  checkCaster<std::complex<float>, std::complex<double>>();
  checkCaster<std::complex<double>, std::complex<double>>();
}

BOOST_AUTO_TEST_CASE(cast_string_test) {
  checkCaster<std::string, std::string>();
}

BOOST_AUTO_TEST_CASE(cast_any_test) {
  checkCasterFromAny<short, int>();
  checkCasterFromAny<int, int>();
  checkCasterFromAny<long, long long>();
  checkCasterFromAny<float, double>();
  checkCasterFromAny<std::string, std::string>();
}

template <typename T>
void checkAnyEqual(boost::any value, T expected) {
  BOOST_CHECK_EQUAL(boost::any_cast<T>(value), expected);
}

BOOST_AUTO_TEST_CASE(any_cast_test) {
  Record<int> int_record { "KEY", 1 };
  Record<boost::any> any_record(int_record);
  Record<int> int2_record(any_record);
  Record<bool> bool_record(int_record);
  BOOST_CHECK_EQUAL(bool_record.value, int_record.value);
}

BOOST_AUTO_TEST_CASE(vector_of_any_test) {
  Record<std::string> str_record("STRING", "HEY!");
  Record<bool> boo_record("BOOL", false);
  Record<std::complex<float>> com_record("COMPLEX", { 1.F, 2.F });
  std::vector<Record<boost::any>> vec;
  vec.emplace_back(str_record);
  vec.emplace_back(boo_record);
  vec.emplace_back(com_record);
  checkAnyEqual<std::string>(vec[0].value, str_record);
  checkAnyEqual<bool>(vec[1].value, boo_record);
  checkAnyEqual<std::complex<float>>(vec[2].value, com_record);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
