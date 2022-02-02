// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsData/TestRecord.h"
#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HeaderWrapper_test)

//-----------------------------------------------------------------------------

const double atol = 1e-4;

template <typename T>
void checkClose(T value, T expected) {
  BOOST_TEST(value == expected);
}

template <>
void checkClose(float value, float expected) {
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template <>
void checkClose(double value, double expected) {
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template <>
void checkClose(std::complex<float> value, std::complex<float> expected) {
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template <>
void checkClose(std::complex<double> value, std::complex<double> expected) {
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template <typename T>
void checkRecordIsReadBack(const std::string& label) {
  Fits::Test::MinimalFile file;
  T value = Fits::Test::generateRandomValue<T>();
  std::string keyword = label.substr(0, 8);
  std::string unit = label.substr(0, 1);
  std::string comment = label.substr(0, 10);
  HeaderIo::writeRecord(file.fptr, Fits::Record<T>(keyword, value, unit, comment));
  const auto parsed = HeaderIo::parseRecord<T>(file.fptr, keyword);
  checkClose(parsed.value, value);
  BOOST_TEST(parsed.unit == unit);
  BOOST_TEST(parsed.comment == comment);
}

template <>
void checkRecordIsReadBack<unsigned long>(const std::string& label) {
  // Known CFITSIO bug: error if value is > max(long)
  (void)(label); // Silent "unused parameter" warning
}

#define RECORD_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_is_read_back_test) { \
    checkRecordIsReadBack<type>(#name); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_IS_READ_BACK_TEST)

BOOST_AUTO_TEST_CASE(empty_value_test) {
  Fits::Test::MinimalFile file;
  Fits::Record<std::string> empty("EMPTY", "", "", "");
  HeaderIo::writeRecord(file.fptr, empty);
  const auto output = HeaderIo::parseRecord<std::string>(file.fptr, empty.keyword);
  BOOST_TEST(output.value == "");
}

BOOST_AUTO_TEST_CASE(missing_keyword_test) {
  Fits::Test::MinimalFile file;
  BOOST_CHECK_THROW(HeaderIo::parseRecord<std::string>(file.fptr, "MISSING"), Fits::FitsError);
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

void checkContains(const std::vector<std::string>& list, const std::vector<std::string>& values) {
  for (const auto& v : values) {
    BOOST_TEST((std::find(list.begin(), list.end(), v) != list.end()));
  }
}

BOOST_FIXTURE_TEST_CASE(struct_io_test, Fits::Test::MinimalFile) {
  RecordList input {{"BOOL", true}, {"INT", 2}, {"DOUBLE", 3.}, {"STRING", "four"}};
  HeaderIo::writeRecords<bool, int, double, std::string>(
      this->fptr,
      {"BOOL", true},
      {"INT", 2},
      {"DOUBLE", 3.},
      {"STRING", "four"});
  std::vector<std::string> keywords {"BOOL", "INT", "DOUBLE", "STRING"};
  const auto categories = ~Euclid::Fits::KeywordCategory::Comment;
  const auto found = HeaderIo::listKeywords(this->fptr, categories);
  checkContains(found, keywords);
  auto records = HeaderIo::parseRecordsAs<RecordList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_TEST(records.b.value == input.b.value);
  BOOST_TEST(records.i.value == input.i.value);
  BOOST_TEST(records.d.value == input.d.value);
  BOOST_TEST(records.s.value == input.s.value);
  auto values = HeaderIo::parseRecordsAs<ValueList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_TEST(values.b == input.b.value);
  BOOST_TEST(values.i == input.i.value);
  BOOST_TEST(values.d == input.d.value);
  BOOST_TEST(values.s == input.s.value);
}

BOOST_FIXTURE_TEST_CASE(several_records_test, Fits::Test::MinimalFile) {
  Fits::Record<std::string> strRecord {"STR", "VALUE"};
  Fits::Record<bool> boolRecord {"BOOL", true};
  Fits::Record<int> intRecord {"INT", 42};
  Fits::Record<float> floatRecord {"FLOAT", 3.14F};
  auto records = std::make_tuple(intRecord, floatRecord);
  HeaderIo::writeRecords(this->fptr, strRecord, boolRecord);
  BOOST_TEST(HeaderIo::parseRecord<std::string>(this->fptr, "STR").value == "VALUE");
  BOOST_TEST(HeaderIo::parseRecord<bool>(this->fptr, "BOOL").value == true);
  HeaderIo::writeRecords(this->fptr, records);
  BOOST_TEST(HeaderIo::parseRecord<int>(this->fptr, "INT").value == 42);
  checkClose(HeaderIo::parseRecord<float>(this->fptr, "FLOAT").value, 3.14F);
  strRecord.value = "NEW";
  boolRecord.value = false;
  std::get<0>(records).value = 43;
  std::get<1>(records).value = 4.14F;
  HeaderIo::updateRecords(this->fptr, strRecord, boolRecord);
  BOOST_TEST(HeaderIo::parseRecord<std::string>(this->fptr, "STR").value == "NEW");
  BOOST_TEST(HeaderIo::parseRecord<bool>(this->fptr, "BOOL").value == false);
  HeaderIo::updateRecords(this->fptr, records);
  BOOST_TEST(HeaderIo::parseRecord<int>(this->fptr, "INT").value == 43);
  checkClose(HeaderIo::parseRecord<float>(this->fptr, "FLOAT").value, 4.14F);
}

template <typename T>
void checkRecordTypeid(T value, const std::vector<std::size_t>& validTypeCodes) {
  Fits::Test::MinimalFile f;
  Fits::Record<T> record {"KEYWORD", value};
  HeaderIo::writeRecord(f.fptr, record);
  const auto& id = HeaderIo::recordTypeid(f.fptr, "KEYWORD").hash_code();
  const auto& it = std::find(validTypeCodes.begin(), validTypeCodes.end(), id);
  BOOST_TEST((it != validTypeCodes.end()));
}

template <typename T>
void checkRecordTypeidMin(const std::vector<std::size_t>& validTypeCodes) {
  checkRecordTypeid(Fits::Test::almostMin<T>(), validTypeCodes);
}

template <typename T>
void checkRecordTypeidMax(const std::vector<std::size_t>& validTypeCodes) {
  checkRecordTypeid(Fits::Test::almostMax<T>(), validTypeCodes);
}

BOOST_AUTO_TEST_CASE(record_type_test) {
  checkRecordTypeidMin<bool>({typeid(bool).hash_code()});
  checkRecordTypeidMin<char>({typeid(char).hash_code(), typeid(unsigned char).hash_code()});
  checkRecordTypeidMin<short>({typeid(short).hash_code()});
  checkRecordTypeidMin<int>({typeid(short).hash_code(), typeid(int).hash_code()});
  checkRecordTypeidMin<long>({typeid(int).hash_code(), typeid(long).hash_code()});
  checkRecordTypeidMin<long long>({typeid(long).hash_code(), typeid(long long).hash_code()});
  checkRecordTypeidMin<float>({typeid(float).hash_code()});
  checkRecordTypeid<double>(Fits::Test::halfMin<double>(), {typeid(double).hash_code()});
  checkRecordTypeidMin<std::complex<float>>({typeid(std::complex<float>).hash_code()});
  checkRecordTypeid<std::complex<double>>(
      Fits::Test::halfMin<std::complex<double>>(),
      {typeid(std::complex<double>).hash_code()});
  checkRecordTypeid<std::string>("VALUE", {typeid(std::string).hash_code()});
  checkRecordTypeidMax<bool>({typeid(bool).hash_code()});
  checkRecordTypeidMax<unsigned char>({typeid(unsigned char).hash_code()});
  checkRecordTypeidMax<unsigned short>({typeid(unsigned short).hash_code()});
  checkRecordTypeidMax<unsigned int>({typeid(unsigned short).hash_code(), typeid(unsigned int).hash_code()});
  checkRecordTypeidMax<unsigned long>({typeid(unsigned int).hash_code(), typeid(unsigned long).hash_code()});
  checkRecordTypeidMax<unsigned long long>({typeid(unsigned long).hash_code(), typeid(unsigned long long).hash_code()});
  checkRecordTypeidMax<float>({typeid(float).hash_code()});
  checkRecordTypeid<double>(Fits::Test::halfMax<double>(), {typeid(double).hash_code()});
  checkRecordTypeidMax<std::complex<float>>({typeid(std::complex<float>).hash_code()});
  checkRecordTypeid<std::complex<double>>(
      Fits::Test::halfMax<std::complex<double>>(),
      {typeid(std::complex<double>).hash_code()});
}

BOOST_FIXTURE_TEST_CASE(parse_vector_and_map_test, Fits::Test::MinimalFile) {
  Fits::Record<short> shortRecord("SHORT", 0);
  Fits::Record<long> longRecord("LONG", 1);
  Fits::Record<long long> longlongRecord("LONGLONG", 2);
  HeaderIo::writeRecords(this->fptr, shortRecord, longRecord, longlongRecord);
  const auto records = HeaderIo::parseRecordVec<long long>(this->fptr, {"SHORT", "LONG", "LONGLONG"});
  BOOST_TEST(records["SHORT"].value == shortRecord);
  BOOST_TEST(records["LONG"].value == longRecord);
  BOOST_TEST(records["LONGLONG"].value == longlongRecord);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
