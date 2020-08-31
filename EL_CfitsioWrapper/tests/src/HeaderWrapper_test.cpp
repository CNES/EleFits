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

#include "EL_FitsData/FitsDataFixture.h"

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (HeaderWrapper_test)

//-----------------------------------------------------------------------------

const double atol = 1e-4;

template<typename T>
void check_close(T value, T expected) {
  BOOST_CHECK_EQUAL(value, expected);
}

template<>
void check_close(float value, float expected) {
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template<>
void check_close(double value, double expected) {
  BOOST_CHECK_CLOSE(value, expected, atol);
}

template<>
void check_close(std::complex<float> value, std::complex<float> expected) {
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template<>
void check_close(std::complex<double> value, std::complex<double> expected) {
  BOOST_CHECK_CLOSE(value.real(), expected.real(), atol);
  BOOST_CHECK_CLOSE(value.imag(), expected.imag(), atol);
}

template<typename T>
void check_record(const std::string& label) {
  FitsIO::Test::MinimalFile file;
  T input = FitsIO::Test::generate_random_value<T>();
  std::string unit = "u_" + label;
  std::string comment = "c_" + label;
  Header::writeRecord(file.fptr, FitsIO::Record<T>(label, input, unit, comment));
  const auto output = Header::parseRecord<T>(file.fptr, label);
  check_close(output.value, input);
  BOOST_CHECK_EQUAL(output.unit, unit);
  BOOST_CHECK_EQUAL(output.comment, comment);
}

#define TEST_RECORD_ALIAS(type, name) \
  BOOST_AUTO_TEST_CASE( name##_test ) { check_record<type>(#name); }

#define TEST_RECORD(type) \
  TEST_RECORD_ALIAS(type, type)

#define TEST_RECORD_UNSIGNED(type) \
  TEST_RECORD_ALIAS(unsigned type, u##type)

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

BOOST_AUTO_TEST_CASE( empty_value_test ) {
  FitsIO::Test::MinimalFile file;
  FitsIO::Record<std::string> empty("EMPTY", "", "", "");
  Header::writeRecord(file.fptr, empty);
  const auto output = Header::parseRecord<std::string>(file.fptr, empty.keyword);
  BOOST_CHECK_EQUAL(output.value, "");
}

BOOST_AUTO_TEST_CASE( missing_keyword_test ) {
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

void check_contains(
    const std::vector<std::string>& list,
    const std::vector<std::string>& values) {
  for(const auto& v : values)
    BOOST_CHECK(std::find(list.begin(), list.end(), v) != list.end());
}

BOOST_AUTO_TEST_CASE( struct_io_test ) {
  FitsIO::Test::MinimalFile file;
  RecordList input {
      { "BOOL", true },
      { "INT", 2 },
      { "DOUBLE", 3.},
      { "STRING", "four"}
  };
  Header::writeRecords<bool, int, double, std::string>(file.fptr,
    { "BOOL", true },
    { "INT", 2 },
    { "DOUBLE", 3.},
    { "STRING", "four"});
  std::vector<std::string> keywords { "BOOL", "INT", "DOUBLE", "STRING" };
  const auto found = Header::listKeywords(file.fptr);
  check_contains(found, keywords);
  auto records = Header::parseRecordsAs<RecordList, bool, int, double, std::string>
      (file.fptr, keywords);
  BOOST_CHECK_EQUAL(records.b.value, input.b.value);
  BOOST_CHECK_EQUAL(records.i.value, input.i.value);
  BOOST_CHECK_EQUAL(records.d.value, input.d.value);
  BOOST_CHECK_EQUAL(records.s.value, input.s.value);
  auto values = Header::parseRecordsAs<ValueList, bool, int, double, std::string>
      (file.fptr, keywords);
  BOOST_CHECK_EQUAL(values.b, input.b.value);
  BOOST_CHECK_EQUAL(values.i, input.i.value);
  BOOST_CHECK_EQUAL(values.d, input.d.value);
  BOOST_CHECK_EQUAL(values.s, input.s.value);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
