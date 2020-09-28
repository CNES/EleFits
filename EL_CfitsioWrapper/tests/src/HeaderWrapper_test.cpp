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
void checkRecord(const std::string &label) {
  FitsIO::Test::MinimalFile file;
  T input = FitsIO::Test::generateRandomValue<T>();
  std::string unit = label;
  std::string comment = label;
  Header::writeRecord(file.fptr, FitsIO::Record<T>(label, input, unit, comment));
  const auto output = Header::parseRecord<T>(file.fptr, label);
  checkClose(output.value, input);
  BOOST_CHECK_EQUAL(output.unit, unit);
  BOOST_CHECK_EQUAL(output.comment, comment);
}

#define TEST_RECORD_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE(name##_test) { \
    checkRecord<type>(#name); \
  }

#define TEST_RECORD(type) TEST_RECORD_ALIAS(type, type)

#define TEST_RECORD_UNSIGNED(type) TEST_RECORD_ALIAS(unsigned type, u##type)

TEST_RECORD(bool)
TEST_RECORD(char)
TEST_RECORD(short)
TEST_RECORD(int)
TEST_RECORD(long)
TEST_RECORD_ALIAS(long long, longlong)
TEST_RECORD(float)
TEST_RECORD(double)
TEST_RECORD_ALIAS(std::complex<float>, cfloat)
TEST_RECORD_ALIAS(std::complex<double>, cdouble)
TEST_RECORD_ALIAS(std::string, string)
TEST_RECORD_UNSIGNED(char)
TEST_RECORD_UNSIGNED(short)
TEST_RECORD_UNSIGNED(int)
// TEST_RECORD_UNSIGNED(long) //TODO random error: wait for CFitsIO feedback
TEST_RECORD_ALIAS(unsigned long long, ulonglong)

BOOST_AUTO_TEST_CASE(empty_value_test) {
  FitsIO::Test::MinimalFile file;
  FitsIO::Record<std::string> empty("EMPTY", "", "", "");
  Header::writeRecord(file.fptr, empty);
  const auto output = Header::parseRecord<std::string>(file.fptr, empty.keyword);
  BOOST_CHECK_EQUAL(output.value, "");
}

BOOST_AUTO_TEST_CASE(missing_keyword_test) {
  FitsIO::Test::MinimalFile file;
  BOOST_CHECK_THROW(Header::parseRecord<std::string>(file.fptr, "MISSING"), std::runtime_error);
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
  const auto found = Header::listValuedKeywords(this->fptr);
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
void checkRecordTypecode(T value, int expectedTypecode) {
  FitsIO::Test::MinimalFile f;
  FitsIO::Record<T> record { "KEYWORD", value };
  Header::writeRecord(f.fptr, record);
  BOOST_CHECK_EQUAL(Header::recordTypecode(f.fptr, "KEYWORD"), expectedTypecode);
}

template <typename T>
void checkRecordTypecodeMin(int expectedTypecode) {
  checkRecordTypecode(FitsIO::Test::almostMin<T>(), expectedTypecode);
}

template <typename T>
void checkRecordTypecodeMax(int expectedTypecode) {
  checkRecordTypecode(FitsIO::Test::almostMax<T>(), expectedTypecode);
}

BOOST_AUTO_TEST_CASE(record_type_test) {
  checkRecordTypecode<bool>(true, TLOGICAL);
  checkRecordTypecodeMin<char>(TSBYTE);
  checkRecordTypecodeMin<short>(TSHORT);
  checkRecordTypecodeMin<int>(TINT); // TODO long?
  // checkRecordTypecodeMin<long>(TLONG); // TODO fix
  checkRecordTypecodeMin<long long>(TLONGLONG);
  checkRecordTypecodeMax<float>(TFLOAT);
  checkRecordTypecodeMin<double>(TDOUBLE);
  checkRecordTypecode<std::complex<float>>(
      { FitsIO::Test::almostMin<float>(), FitsIO::Test::almostMax<float>() },
      TCOMPLEX);
  checkRecordTypecode<std::complex<double>>(
      { FitsIO::Test::almostMin<double>(), FitsIO::Test::almostMax<double>() },
      TDBLCOMPLEX);
  checkRecordTypecode<std::string>("VALUE", TSTRING);
  checkRecordTypecodeMax<unsigned char>(TBYTE);
  checkRecordTypecodeMax<unsigned short>(TUSHORT);
  checkRecordTypecodeMax<unsigned int>(TUINT); // TODO long?
  // checkRecordTypecodeMax<unsigned long>(TULONG); // TODO fix
  checkRecordTypecodeMax<unsigned long long>(TULONGLONG);
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
