// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Record.h"
#include "EleFitsData/TestRecord.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Record_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(full_init_test)
{
  Record<int> full {"FULL", 4, "m", "Full"};

  BOOST_TEST(full.keyword == "FULL");
  BOOST_TEST(full.value == 4);
  BOOST_TEST(full.unit == "m");
  BOOST_TEST(full.comment == "Full");
}

BOOST_AUTO_TEST_CASE(unit_init_test)
{
  Record<int> unit {"UNIT", 3, "m"};

  BOOST_TEST(unit.keyword == "UNIT");
  BOOST_TEST(unit.value == 3);
  BOOST_TEST(unit.unit == "m");
  BOOST_TEST(unit.comment == "");
}

BOOST_AUTO_TEST_CASE(mini_init_test)
{
  Record<int> mini {"MINI", 2};

  BOOST_TEST(mini.keyword == "MINI");
  BOOST_TEST(mini.value == 2);
  BOOST_TEST(mini.unit == "");
  BOOST_TEST(mini.comment == "");
}

template <typename T>
void check_record_copy(const std::string& keyword)
{
  Record<T> original {keyword, Test::generate_random_value<T>()};
  Record<T> copy(original);
  Record<T> assigned;
  assigned = original;
  BOOST_TEST((copy == original));
  BOOST_TEST((assigned == original));
}

#define RECORD_COPY_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_copy_test) \
  { \
    check_record_copy<type>(#name); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_COPY_TEST)

BOOST_AUTO_TEST_CASE(raw_comment_test)
{
  Record<int> no_unit {"V", 1, "", "Speed"};
  Record<int> unit {"V", 1, "m", "Speed"};
  Record<int> unit_in_comment {"V", 1, "", "[m] Speed"};

  BOOST_TEST(no_unit.raw_comment() == "Speed");
  BOOST_TEST(unit.raw_comment() == "[m] Speed");
  BOOST_TEST(unit_in_comment.raw_comment() == "[m] Speed");
}

template <typename T>
void check_equal(T value, T expected)
{
  BOOST_TEST(value == expected);
}

template <typename TV, typename TE>
void check_approx(TV value, TE expected)
{
  BOOST_TEST(Test::approx(value, static_cast<TV>(expected)));
}

template <typename TV, typename TE>
void check_approx(std::complex<TV> value, std::complex<TE> expected)
{
  check_approx(value.real(), expected.real());
  check_approx(value.imag(), expected.imag());
}

template <>
void check_approx(std::string value, std::string expected)
{
  BOOST_TEST(value == expected);
}

template <typename T>
void check_record_slicing()
{
  T v = Test::generate_random_value<T>();
  Record<T> r("KEY", v);
  check_equal<T>(r, v);
}

#define RECORD_SLICING_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_slicing_test) \
  { \
    check_record_slicing<type>(); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_SLICING_TEST)

BOOST_AUTO_TEST_CASE(long_string_values_are_detected_test)
{
  const std::string short_string = "XS";
  const std::string long_string = std::string(99, 'X') + 'L';
  BOOST_TEST(short_string.length() <= 80);
  BOOST_TEST(long_string.length() > 80);
  const Record<std::string> short_string_record("STRINGXS", short_string);
  BOOST_TEST(not short_string_record.has_long_string_value());
  const Record<std::string> long_string_record("STRINGXL", long_string);
  BOOST_TEST(long_string_record.has_long_string_value());
  const Record<VariantValue> short_any_record("ANYXS", short_string);
  BOOST_TEST(not short_any_record.has_long_string_value());
  const Record<VariantValue> long_any_record("ANYXL", long_string);
  BOOST_TEST(long_any_record.has_long_string_value());
}

template <typename TFrom, typename TTo>
void check_record_casting()
{
  TFrom input = Test::generate_random_value<TFrom>();
  TTo output = Record<TTo>::cast(input);
  check_approx(output, input);
}

template <typename TFrom, typename TTo>
void check_record_casting_from_any()
{
  TFrom value = Test::generate_random_value<TFrom>();
  VariantValue input(value);
  TTo output = Record<TTo>::cast(input);
  check_approx(output, value);
}

BOOST_AUTO_TEST_CASE(scalar_record_casting_test)
{
  check_record_casting<bool, long long>();
  check_record_casting<char, long>();
  check_record_casting<short, int>();
  check_record_casting<float, double>();
}

BOOST_AUTO_TEST_CASE(complex_record_casting_test)
{
  check_record_casting<std::complex<float>, std::complex<float>>();
  check_record_casting<std::complex<float>, std::complex<double>>();
  check_record_casting<std::complex<double>, std::complex<double>>();
}

BOOST_AUTO_TEST_CASE(string_record_casting_test)
{
  check_record_casting<std::string, std::string>();
}

BOOST_AUTO_TEST_CASE(any_record_casting_test)
{
  check_record_casting_from_any<short, int>();
  check_record_casting_from_any<int, int>();
  check_record_casting_from_any<long, long long>();
  check_record_casting_from_any<float, double>();
  check_record_casting_from_any<std::string, std::string>();
  check_record_casting_from_any<std::complex<float>, std::complex<float>>();
}

template <typename T>
void check_any_equal(VariantValue value, T expected)
{
  BOOST_TEST(boost::any_cast<T>(value) == expected);
}

BOOST_AUTO_TEST_CASE(vector_of_any_is_built_and_cast_back_test)
{
  Record<std::string> str_record("STRING", "HEY!");
  Record<bool> boo_record("BOOL", false);
  Record<std::complex<float>> com_record("COMPLEX", {1.F, 2.F});
  std::vector<Record<VariantValue>> vec;
  vec.emplace_back(str_record);
  vec.emplace_back(boo_record);
  vec.emplace_back(com_record);
  check_any_equal<std::string>(vec[0].value, str_record);
  check_any_equal<bool>(vec[1].value, boo_record);
  check_any_equal<std::complex<float>>(vec[2].value, com_record);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
