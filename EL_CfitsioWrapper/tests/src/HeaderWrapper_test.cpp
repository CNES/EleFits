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

#include "EL_FitsData/TestRecord.h"

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HeaderWrapper_test)

//-----------------------------------------------------------------------------

const double atol = 1e-4;

template <typename T>
void checkClose(T value, T expected) {
  BOOST_CHECK_EQUAL(value, expected);
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
void checkRecordIsReadBack(const std::string &label) {
  FitsIO::Test::MinimalFile file;
  T value = FitsIO::Test::generateRandomValue<T>();
  std::string keyword = label.substr(0, 8);
  std::string unit = label.substr(0, 1);
  std::string comment = label.substr(0, 10);
  Header::writeRecord(file.fptr, FitsIO::Record<T>(keyword, value, unit, comment));
  const auto parsed = Header::parseRecord<T>(file.fptr, keyword);
  checkClose(parsed.value, value);
  BOOST_CHECK_EQUAL(parsed.unit, unit);
  BOOST_CHECK_EQUAL(parsed.comment, comment);
}

template <>
void checkRecordIsReadBack<unsigned long>(const std::string &label) {
  // Known CFitsIO bug: error if value is > max(long)
  (void)(label); // Silent "unused parameter" warning
}

#define RECORD_IS_READ_BACK_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_is_read_back_test) { \
    checkRecordIsReadBack<type>(#name); \
  }

EL_FITSIO_FOREACH_RECORD_TYPE(RECORD_IS_READ_BACK_TEST)

BOOST_AUTO_TEST_CASE(empty_value_test) {
  FitsIO::Test::MinimalFile file;
  FitsIO::Record<std::string> empty("EMPTY", "", "", "");
  Header::writeRecord(file.fptr, empty);
  const auto output = Header::parseRecord<std::string>(file.fptr, empty.keyword);
  BOOST_CHECK_EQUAL(output.value, "");
}

BOOST_AUTO_TEST_CASE(missing_keyword_test) {
  FitsIO::Test::MinimalFile file;
  BOOST_CHECK_THROW(Header::parseRecord<std::string>(file.fptr, "MISSING"), FitsIO::FitsIOError);
}

struct RecordList {
  FitsIO::Record<bool> b;
  FitsIO::Record<int> i;
  FitsIO::Record<double> d;
  FitsIO::Record<std::string> s;
};

struct ValueList {
  bool b;
  int i;
  double d;
  std::string s;
};

void checkContains(const std::vector<std::string> &list, const std::vector<std::string> &values) {
  for (const auto &v : values) {
    BOOST_CHECK(std::find(list.begin(), list.end(), v) != list.end());
  }
}

BOOST_FIXTURE_TEST_CASE(struct_io_test, FitsIO::Test::MinimalFile) {
  RecordList input { { "BOOL", true }, { "INT", 2 }, { "DOUBLE", 3. }, { "STRING", "four" } };
  Header::writeRecords<bool, int, double, std::string>(
      this->fptr,
      { "BOOL", true },
      { "INT", 2 },
      { "DOUBLE", 3. },
      { "STRING", "four" });
  std::vector<std::string> keywords { "BOOL", "INT", "DOUBLE", "STRING" };
  const auto categories = ~Euclid::FitsIO::KeywordCategory::Comment;
  const auto found = Header::listKeywords(this->fptr, categories);
  checkContains(found, keywords);
  auto records = Header::parseRecordsAs<RecordList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_CHECK_EQUAL(records.b.value, input.b.value);
  BOOST_CHECK_EQUAL(records.i.value, input.i.value);
  BOOST_CHECK_EQUAL(records.d.value, input.d.value);
  BOOST_CHECK_EQUAL(records.s.value, input.s.value);
  auto values = Header::parseRecordsAs<ValueList, bool, int, double, std::string>(this->fptr, keywords);
  BOOST_CHECK_EQUAL(values.b, input.b.value);
  BOOST_CHECK_EQUAL(values.i, input.i.value);
  BOOST_CHECK_EQUAL(values.d, input.d.value);
  BOOST_CHECK_EQUAL(values.s, input.s.value);
}

BOOST_FIXTURE_TEST_CASE(several_records_test, FitsIO::Test::MinimalFile) {
  FitsIO::Record<std::string> strRecord { "STR", "VALUE" };
  FitsIO::Record<bool> boolRecord { "BOOL", true };
  FitsIO::Record<int> intRecord { "INT", 42 };
  FitsIO::Record<float> floatRecord { "FLOAT", 3.14F };
  auto records = std::make_tuple(intRecord, floatRecord);
  Header::writeRecords(this->fptr, strRecord, boolRecord);
  BOOST_CHECK_EQUAL(Header::parseRecord<std::string>(this->fptr, "STR").value, "VALUE");
  BOOST_CHECK_EQUAL(Header::parseRecord<bool>(this->fptr, "BOOL").value, true);
  Header::writeRecords(this->fptr, records);
  BOOST_CHECK_EQUAL(Header::parseRecord<int>(this->fptr, "INT").value, 42);
  checkClose(Header::parseRecord<float>(this->fptr, "FLOAT").value, 3.14F);
  strRecord.value = "NEW";
  boolRecord.value = false;
  std::get<0>(records).value = 43;
  std::get<1>(records).value = 4.14F;
  Header::updateRecords(this->fptr, strRecord, boolRecord);
  BOOST_CHECK_EQUAL(Header::parseRecord<std::string>(this->fptr, "STR").value, "NEW");
  BOOST_CHECK_EQUAL(Header::parseRecord<bool>(this->fptr, "BOOL").value, false);
  Header::updateRecords(this->fptr, records);
  BOOST_CHECK_EQUAL(Header::parseRecord<int>(this->fptr, "INT").value, 43);
  checkClose(Header::parseRecord<float>(this->fptr, "FLOAT").value, 4.14F);
}

template <typename T>
void checkRecordTypeid(T value, const std::vector<std::size_t> &validTypeCodes) {
  FitsIO::Test::MinimalFile f;
  FitsIO::Record<T> record { "KEYWORD", value };
  Header::writeRecord(f.fptr, record);
  const auto &id = Header::recordTypeid(f.fptr, "KEYWORD").hash_code();
  const auto &it = std::find(validTypeCodes.begin(), validTypeCodes.end(), id);
  BOOST_CHECK(it != validTypeCodes.end());
}

template <typename T>
void checkRecordTypeidMin(const std::vector<std::size_t> &validTypeCodes) {
  checkRecordTypeid(FitsIO::Test::almostMin<T>(), validTypeCodes);
}

template <typename T>
void checkRecordTypeidMax(const std::vector<std::size_t> &validTypeCodes) {
  checkRecordTypeid(FitsIO::Test::almostMax<T>(), validTypeCodes);
}

BOOST_AUTO_TEST_CASE(record_type_test) {
  checkRecordTypeidMin<bool>({ typeid(bool).hash_code() });
  checkRecordTypeidMin<char>({ typeid(char).hash_code(), typeid(unsigned char).hash_code() });
  checkRecordTypeidMin<short>({ typeid(short).hash_code() });
  checkRecordTypeidMin<int>({ typeid(short).hash_code(), typeid(int).hash_code() });
  checkRecordTypeidMin<long>({ typeid(int).hash_code(), typeid(long).hash_code() });
  checkRecordTypeidMin<long long>({ typeid(long).hash_code(), typeid(long long).hash_code() });
  checkRecordTypeidMin<float>({ typeid(float).hash_code() });
  checkRecordTypeid<double>(FitsIO::Test::halfMin<double>(), { typeid(double).hash_code() });
  checkRecordTypeidMin<std::complex<float>>({ typeid(std::complex<float>).hash_code() });
  checkRecordTypeid<std::complex<double>>(
      FitsIO::Test::halfMin<std::complex<double>>(),
      { typeid(std::complex<double>).hash_code() });
  checkRecordTypeid<std::string>("VALUE", { typeid(std::string).hash_code() });
  checkRecordTypeidMax<bool>({ typeid(bool).hash_code() });
  checkRecordTypeidMax<unsigned char>({ typeid(unsigned char).hash_code() });
  checkRecordTypeidMax<unsigned short>({ typeid(unsigned short).hash_code() });
  checkRecordTypeidMax<unsigned int>({ typeid(unsigned short).hash_code(), typeid(unsigned int).hash_code() });
  checkRecordTypeidMax<unsigned long>({ typeid(unsigned int).hash_code(), typeid(unsigned long).hash_code() });
  checkRecordTypeidMax<unsigned long long>(
      { typeid(unsigned long).hash_code(), typeid(unsigned long long).hash_code() });
  checkRecordTypeidMax<float>({ typeid(float).hash_code() });
  checkRecordTypeid<double>(FitsIO::Test::halfMax<double>(), { typeid(double).hash_code() });
  checkRecordTypeidMax<std::complex<float>>({ typeid(std::complex<float>).hash_code() });
  checkRecordTypeid<std::complex<double>>(
      FitsIO::Test::halfMax<std::complex<double>>(),
      { typeid(std::complex<double>).hash_code() });
}

BOOST_FIXTURE_TEST_CASE(parse_vector_and_map_test, FitsIO::Test::MinimalFile) {
  FitsIO::Record<short> shortRecord("SHORT", 0);
  FitsIO::Record<long> longRecord("LONG", 1);
  FitsIO::Record<long long> longlongRecord("LONGLONG", 2);
  Header::writeRecords(this->fptr, shortRecord, longRecord, longlongRecord);
  const auto records = Header::parseRecordVector<long long>(this->fptr, { "SHORT", "LONG", "LONGLONG" });
  BOOST_CHECK_EQUAL(records["SHORT"].value, shortRecord);
  BOOST_CHECK_EQUAL(records["LONG"].value, longRecord);
  BOOST_CHECK_EQUAL(records["LONGLONG"].value, longlongRecord);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
