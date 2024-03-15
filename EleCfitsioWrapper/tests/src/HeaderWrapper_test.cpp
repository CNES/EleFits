// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsData/TestRecord.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HeaderWrapper_test)

//-----------------------------------------------------------------------------

const double atol = 1e-4;

template <typename T>
void check_close(T value, T expected)
{
  BOOST_TEST(value == expected);
}

template <>
void check_close(float value, float expected)
{
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template <>
void check_close(double value, double expected)
{
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template <>
void check_close(std::complex<float> value, std::complex<float> expected)
{
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template <>
void check_close(std::complex<double> value, std::complex<double> expected)
{
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template <typename T>
void check_record_is_read_back(const std::string& label)
{
  Fits::Test::MinimalFile file;
  T value = Fits::Test::generate_random_value<T>();
  std::string keyword = label.substr(0, 8);
  std::string unit = label.substr(0, 1);
  std::string comment = label.substr(0, 10);
  HeaderIo::write_record(file.fptr, Fits::Record<T>(keyword, value, unit, comment));
  const auto parsed = HeaderIo::parse_record<T>(file.fptr, keyword);
  check_close(parsed.value, value);
  BOOST_TEST(parsed.unit == unit);
  BOOST_TEST(parsed.comment == comment);
}

template <>
void check_record_is_read_back<unsigned long>(const std::string& label)
{
  // Known CFITSIO bug: error if value is > max(long)
  (void)(label); // Silent "unused parameter" warning
}

#define RECORD_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_is_read_back_test) \
  { \
    check_record_is_read_back<type>(#name); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_IS_READ_BACK_TEST)

BOOST_AUTO_TEST_CASE(empty_value_test)
{
  Fits::Test::MinimalFile file;
  Fits::Record<std::string> empty("EMPTY", "", "", "");
  HeaderIo::write_record(file.fptr, empty);
  const auto output = HeaderIo::parse_record<std::string>(file.fptr, empty.keyword);
  BOOST_TEST(output.value == "");
}

BOOST_AUTO_TEST_CASE(missing_keyword_test)
{
  Fits::Test::MinimalFile file;
  BOOST_CHECK_THROW(HeaderIo::parse_record<std::string>(file.fptr, "MISSING"), Fits::FitsError);
}

struct RecordList {
  Fits::Record<bool> b;
  Fits::Record<int> i;
  Fits::Record<double> d;
  Fits::Record<std::string> s;
};

struct ValueList {
  bool b;
  int i;
  double d;
  std::string s;
};

void check_contains(const std::vector<std::string>& list, const std::vector<std::string>& values)
{
  for (const auto& v : values) {
    BOOST_TEST((std::find(list.begin(), list.end(), v) != list.end()));
  }
}

BOOST_FIXTURE_TEST_CASE(struct_io_test, Fits::Test::MinimalFile)
{
  RecordList input {{"BOOL", true}, {"INT", 2}, {"DOUBLE", 3.}, {"STRING", "four"}};
  HeaderIo::write_records<bool, int, double, std::string>(
      this->fptr,
      {"BOOL", true},
      {"INT", 2},
      {"DOUBLE", 3.},
      {"STRING", "four"});
  std::vector<std::string> keywords {"BOOL", "INT", "DOUBLE", "STRING"};
  const auto categories = ~Fits::KeywordCategory::Comment;
  const auto found = HeaderIo::list_keywords(this->fptr, categories);
  check_contains(found, keywords);
  auto records = HeaderIo::parse_records_as<RecordList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_TEST(records.b.value == input.b.value);
  BOOST_TEST(records.i.value == input.i.value);
  BOOST_TEST(records.d.value == input.d.value);
  BOOST_TEST(records.s.value == input.s.value);
  auto values = HeaderIo::parse_records_as<ValueList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_TEST(values.b == input.b.value);
  BOOST_TEST(values.i == input.i.value);
  BOOST_TEST(values.d == input.d.value);
  BOOST_TEST(values.s == input.s.value);
}

BOOST_FIXTURE_TEST_CASE(several_records_test, Fits::Test::MinimalFile)
{
  Fits::Record<std::string> str_ecord {"STR", "VALUE"};
  Fits::Record<bool> bool_record {"BOOL", true};
  Fits::Record<int> int_record {"INT", 42};
  Fits::Record<float> float_record {"FLOAT", 3.14F};
  auto records = std::make_tuple(int_record, float_record);
  HeaderIo::write_records(this->fptr, str_ecord, bool_record);
  BOOST_TEST(HeaderIo::parse_record<std::string>(this->fptr, "STR").value == "VALUE");
  BOOST_TEST(HeaderIo::parse_record<bool>(this->fptr, "BOOL").value == true);
  HeaderIo::write_records(this->fptr, records);
  BOOST_TEST(HeaderIo::parse_record<int>(this->fptr, "INT").value == 42);
  check_close(HeaderIo::parse_record<float>(this->fptr, "FLOAT").value, 3.14F);
  str_ecord.value = "NEW";
  bool_record.value = false;
  std::get<0>(records).value = 43;
  std::get<1>(records).value = 4.14F;
  HeaderIo::update_records(this->fptr, str_ecord, bool_record);
  BOOST_TEST(HeaderIo::parse_record<std::string>(this->fptr, "STR").value == "NEW");
  BOOST_TEST(HeaderIo::parse_record<bool>(this->fptr, "BOOL").value == false);
  HeaderIo::update_records(this->fptr, records);
  BOOST_TEST(HeaderIo::parse_record<int>(this->fptr, "INT").value == 43);
  check_close(HeaderIo::parse_record<float>(this->fptr, "FLOAT").value, 4.14F);
}

template <typename T>
void check_record_typeid(T value, const std::vector<std::size_t>& valid_type_codes)
{
  Fits::Test::MinimalFile f;
  Fits::Record<T> record {"KEYWORD", value};
  HeaderIo::write_record(f.fptr, record);
  const auto& id = HeaderIo::record_typeid(f.fptr, "KEYWORD").hash_code();
  const auto& it = std::find(valid_type_codes.begin(), valid_type_codes.end(), id);
  BOOST_TEST((it != valid_type_codes.end()));
}

template <typename T>
void check_record_typeid_min(const std::vector<std::size_t>& valid_type_codes)
{
  check_record_typeid(Fits::Test::almost_min<T>(), valid_type_codes);
}

template <typename T>
void check_record_typeid_max(const std::vector<std::size_t>& valid_type_codes)
{
  check_record_typeid(Fits::Test::almost_max<T>(), valid_type_codes);
}

BOOST_AUTO_TEST_CASE(record_type_test)
{
  check_record_typeid_min<bool>({typeid(bool).hash_code()});
  check_record_typeid_min<char>({typeid(char).hash_code(), typeid(unsigned char).hash_code()});
  check_record_typeid_min<short>({typeid(short).hash_code()});
  check_record_typeid_min<int>({typeid(short).hash_code(), typeid(int).hash_code()});
  check_record_typeid_min<long>({typeid(int).hash_code(), typeid(long).hash_code()});
  check_record_typeid_min<long long>({typeid(long).hash_code(), typeid(long long).hash_code()});
  check_record_typeid_min<float>({typeid(float).hash_code()});
  check_record_typeid<double>(Fits::Test::half_min<double>(), {typeid(double).hash_code()});
  check_record_typeid_min<std::complex<float>>({typeid(std::complex<float>).hash_code()});
  check_record_typeid<std::complex<double>>(
      Fits::Test::half_min<std::complex<double>>(),
      {typeid(std::complex<double>).hash_code()});
  check_record_typeid<std::string>("VALUE", {typeid(std::string).hash_code()});
  check_record_typeid_max<bool>({typeid(bool).hash_code()});
  check_record_typeid_max<unsigned char>({typeid(unsigned char).hash_code()});
  check_record_typeid_max<unsigned short>({typeid(unsigned short).hash_code()});
  check_record_typeid_max<unsigned int>({typeid(unsigned short).hash_code(), typeid(unsigned int).hash_code()});
  check_record_typeid_max<unsigned long>({typeid(unsigned int).hash_code(), typeid(unsigned long).hash_code()});
  check_record_typeid_max<unsigned long long>(
      {typeid(unsigned long).hash_code(), typeid(unsigned long long).hash_code()});
  check_record_typeid_max<float>({typeid(float).hash_code()});
  check_record_typeid<double>(Fits::Test::half_max<double>(), {typeid(double).hash_code()});
  check_record_typeid_max<std::complex<float>>({typeid(std::complex<float>).hash_code()});
  check_record_typeid<std::complex<double>>(
      Fits::Test::half_max<std::complex<double>>(),
      {typeid(std::complex<double>).hash_code()});
}

BOOST_FIXTURE_TEST_CASE(parse_vector_and_map_test, Fits::Test::MinimalFile)
{
  Fits::Record<short> short_record("SHORT", 0);
  Fits::Record<long> long_record("LONG", 1);
  Fits::Record<long long> longlong_record("LONGLONG", 2);
  HeaderIo::write_records(this->fptr, short_record, long_record, longlong_record);
  const auto records = HeaderIo::parse_record_vec<long long>(this->fptr, {"SHORT", "LONG", "LONGLONG"});
  BOOST_TEST(records["SHORT"].value == short_record);
  BOOST_TEST(records["LONG"].value == long_record);
  BOOST_TEST(records["LONGLONG"].value == longlong_record);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
