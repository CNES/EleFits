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

#include "EleFitsData/Record.h"
#include "EleFitsData/TestRecord.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Record_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(full_init_test) {

  Record<int> full { "FULL", 4, "m", "Full" };

  BOOST_TEST(full.keyword == "FULL");
  BOOST_TEST(full.value == 4);
  BOOST_TEST(full.unit == "m");
  BOOST_TEST(full.comment == "Full");
}

BOOST_AUTO_TEST_CASE(unit_init_test) {

  Record<int> unit { "UNIT", 3, "m" };

  BOOST_TEST(unit.keyword == "UNIT");
  BOOST_TEST(unit.value == 3);
  BOOST_TEST(unit.unit == "m");
  BOOST_TEST(unit.comment == "");
}

BOOST_AUTO_TEST_CASE(mini_init_test) {

  Record<int> mini { "MINI", 2 };

  BOOST_TEST(mini.keyword == "MINI");
  BOOST_TEST(mini.value == 2);
  BOOST_TEST(mini.unit == "");
  BOOST_TEST(mini.comment == "");
}

template <typename T>
void checkRecordCopy(const std::string& keyword) {
  Record<T> original { keyword, Test::generateRandomValue<T>() };
  Record<T> copy(original);
  Record<T> assigned;
  assigned = original;
  BOOST_TEST((copy == original));
  BOOST_TEST((assigned == original));
}

#define RECORD_COPY_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_copy_test) { \
    checkRecordCopy<type>(#name); \
  }

EL_FITSIO_FOREACH_RECORD_TYPE(RECORD_COPY_TEST)

BOOST_AUTO_TEST_CASE(raw_comment_test) {

  Record<int> noUnit { "V", 1, "", "Speed" };
  Record<int> unit { "V", 1, "m", "Speed" };
  Record<int> unitInComment { "V", 1, "", "[m] Speed" };

  BOOST_TEST(noUnit.rawComment() == "Speed");
  BOOST_TEST(unit.rawComment() == "[m] Speed");
  BOOST_TEST(unitInComment.rawComment() == "[m] Speed");
}

template <typename T>
void checkEqual(T value, T expected) {
  BOOST_TEST(value == expected);
}

template <typename TV, typename TE>
void checkApprox(TV value, TE expected) {
  BOOST_TEST(Test::approx(value, static_cast<TV>(expected)));
}

template <typename TV, typename TE>
void checkApprox(std::complex<TV> value, std::complex<TE> expected) {
  checkApprox(value.real(), expected.real());
  checkApprox(value.imag(), expected.imag());
}

template <>
void checkApprox(std::string value, std::string expected) {
  BOOST_TEST(value == expected);
}

template <typename T>
void checkRecordSlicing() {
  T v = Test::generateRandomValue<T>();
  Record<T> r("KEY", v);
  checkEqual<T>(r, v);
}

#define RECORD_SLICING_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_slicing_test) { \
    checkRecordSlicing<type>(); \
  }

EL_FITSIO_FOREACH_RECORD_TYPE(RECORD_SLICING_TEST)

BOOST_AUTO_TEST_CASE(long_string_values_are_detected_test) {
  const std::string shortString = "XS";
  const std::string longString = std::string(99, 'X') + 'L';
  BOOST_TEST(shortString.length() <= 80);
  BOOST_TEST(longString.length() > 80);
  const Record<std::string> shortStringRecord("STRINGXS", shortString);
  BOOST_TEST(not shortStringRecord.hasLongStringValue());
  const Record<std::string> longStringRecord("STRINGXL", longString);
  BOOST_TEST(longStringRecord.hasLongStringValue());
  const Record<VariantValue> shortAnyRecord("ANYXS", shortString);
  BOOST_TEST(not shortAnyRecord.hasLongStringValue());
  const Record<VariantValue> longAnyRecord("ANYXL", longString);
  BOOST_TEST(longAnyRecord.hasLongStringValue());
}

template <typename TFrom, typename TTo>
void checkRecordCasting() {
  TFrom input = Test::generateRandomValue<TFrom>();
  TTo output = Record<TTo>::cast(input);
  checkApprox(output, input);
}

template <typename TFrom, typename TTo>
void checkRecordCastingFromAny() {
  TFrom value = Test::generateRandomValue<TFrom>();
  VariantValue input(value);
  TTo output = Record<TTo>::cast(input);
  checkApprox(output, value);
}

BOOST_AUTO_TEST_CASE(scalar_record_casting_test) {
  checkRecordCasting<bool, long long>();
  checkRecordCasting<char, long>();
  checkRecordCasting<short, int>();
  checkRecordCasting<float, double>();
}

BOOST_AUTO_TEST_CASE(complex_record_casting_test) {
  checkRecordCasting<std::complex<float>, std::complex<float>>();
  checkRecordCasting<std::complex<float>, std::complex<double>>();
  checkRecordCasting<std::complex<double>, std::complex<double>>();
}

BOOST_AUTO_TEST_CASE(string_record_casting_test) {
  checkRecordCasting<std::string, std::string>();
}

BOOST_AUTO_TEST_CASE(any_record_casting_test) {
  checkRecordCastingFromAny<short, int>();
  checkRecordCastingFromAny<int, int>();
  checkRecordCastingFromAny<long, long long>();
  checkRecordCastingFromAny<float, double>();
  checkRecordCastingFromAny<std::string, std::string>();
  checkRecordCastingFromAny<std::complex<float>, std::complex<float>>();
}

template <typename T>
void checkAnyEqual(VariantValue value, T expected) {
  BOOST_TEST(boost::any_cast<T>(value) == expected);
}

BOOST_AUTO_TEST_CASE(vector_of_any_is_built_and_cast_back_test) {
  Record<std::string> str_record("STRING", "HEY!");
  Record<bool> boo_record("BOOL", false);
  Record<std::complex<float>> com_record("COMPLEX", { 1.F, 2.F });
  std::vector<Record<VariantValue>> vec;
  vec.emplace_back(str_record);
  vec.emplace_back(boo_record);
  vec.emplace_back(com_record);
  checkAnyEqual<std::string>(vec[0].value, str_record);
  checkAnyEqual<bool>(vec[1].value, boo_record);
  checkAnyEqual<std::complex<float>>(vec[2].value, com_record);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
