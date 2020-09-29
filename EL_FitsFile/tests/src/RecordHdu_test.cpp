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

#include "ElementsKernel/Temporary.h"

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsFile/SifFile.h"
#include "EL_FitsFile/FitsFileFixture.h"

#include "EL_FitsFile/RecordHdu.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(RecordHdu_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(continued_str_test, Test::TemporarySifFile) {
  const auto &h = this->header();
  const std::string shortStr = "S";
  const std::string longStr = "This is probably one of the longest strings "
                              "that I have ever written in a serious code.";
  BOOST_CHECK_GT(longStr.length(), FLEN_VALUE);
  h.writeRecord<std::string>("SHORT", shortStr);
  BOOST_CHECK_THROW(h.parseRecord<std::string>("LONGSTRN"), std::exception);
  h.writeRecord<std::string>("LONG", longStr);
  const auto output = h.parseRecord<std::string>("LONG");
  h.parseRecord<std::string>("LONGSTRN");
  BOOST_CHECK_EQUAL(output.value, longStr);
}

BOOST_FIXTURE_TEST_CASE(rename_test, Test::TemporaryMefFile) {
  const auto &h = this->initRecordExt("A");
  BOOST_CHECK_EQUAL(h.index(), 2);
  BOOST_CHECK_EQUAL(h.readName(), "A");
  h.updateName("B");
  BOOST_CHECK_EQUAL(h.readName(), "B");
  h.deleteRecord("EXTNAME");
  BOOST_CHECK_EQUAL(h.readName(), "");
}

BOOST_FIXTURE_TEST_CASE(c_str_record_test, Test::TemporarySifFile) {
  const auto &h = this->header();
  h.writeRecord("C_STR", "1");
  const auto output1 = h.parseRecord<std::string>("C_STR");
  BOOST_CHECK_EQUAL(output1.value, "1");
  h.updateRecord("C_STR", "2");
  const auto output2 = h.parseRecord<std::string>("C_STR");
  BOOST_CHECK_EQUAL(output2.value, "2");
}

BOOST_FIXTURE_TEST_CASE(tuple_write_update_test, Test::TemporarySifFile) {
  const auto &h = this->header();
  Record<short> short_record { "SHORT", 1 };
  Record<long> long_record { "LONG", 1000 };
  auto records = std::make_tuple(short_record, long_record);
  h.writeRecords(records);
  BOOST_CHECK_EQUAL(h.parseRecord<short>("SHORT"), 1);
  BOOST_CHECK_EQUAL(h.parseRecord<long>("LONG"), 1000);
  std::get<0>(records).value = 2;
  std::get<1>(records).value = 2000;
  h.updateRecords(records);
  BOOST_CHECK_EQUAL(h.parseRecord<short>("SHORT"), 2);
  BOOST_CHECK_EQUAL(h.parseRecord<long>("LONG"), 2000);
}

BOOST_FIXTURE_TEST_CASE(vector_of_any_test, Test::TemporarySifFile) {
  const auto &h = this->header();
  std::vector<Record<boost::any>> records;
  records.push_back({ "STRING", std::string("WIDE") });
  records.push_back({ "FLOAT", 3.14F });
  records.push_back({ "INT", 666 });
  h.writeRecords(records);
  auto parsed = h.parseAllRecords<boost::any>();
  BOOST_CHECK_EQUAL(parsed.as<std::string>("STRING").value, "WIDE");
  BOOST_CHECK_EQUAL(parsed.as<int>("INT").value, 666);
  BOOST_CHECK_THROW(parsed.as<std::string>("INT"), std::exception);
}

BOOST_FIXTURE_TEST_CASE(vector_of_any_subset_test, Test::TemporarySifFile) {
  const auto &h = this->header();
  RecordVector<boost::any> records(3);
  records.vector[0] = Record<std::string>("STRING", "WIDE");
  records.vector[1] = Record<float>("FLOAT", 3.14F);
  records.vector[2] = Record<int>("INT", 666);
  h.writeRecords(records, { "FLOAT", "INT" });
  BOOST_CHECK_THROW(h.parseRecord<boost::any>("STRING"), std::exception);
  auto parsed = h.parseRecordVector<boost::any>({ "INT" });
  BOOST_CHECK_EQUAL(parsed.as<int>("INT").value, 666);
  BOOST_CHECK_THROW(parsed.as<float>("FLOAT"), std::exception);
}

BOOST_FIXTURE_TEST_CASE(brackets_in_comment_test, Test::TemporaryMefFile) {
  const auto &primary = this->accessPrimary<>();
  primary.writeRecord("PLAN_ID", 1, "", "[0:1] SOC Planning ID");
  const auto intRecord = primary.parseRecord<int>("PLAN_ID");
  BOOST_CHECK_EQUAL(intRecord.unit, "0:1");
  BOOST_CHECK_EQUAL(intRecord.comment, "SOC Planning ID");
  primary.writeRecord("STRING", std::string("1"), "", "[0:1] SOC Planning ID");
  const auto stringRecord = primary.parseRecord<std::string>("STRING");
  BOOST_CHECK_EQUAL(stringRecord.unit, "0:1");
  BOOST_CHECK_EQUAL(stringRecord.comment, "SOC Planning ID");
  primary.writeRecord("CSTR", "1", "", "[0:1] SOC Planning ID");
  const auto cstrRecord = primary.parseRecord<std::string>("CSTR");
  BOOST_CHECK_EQUAL(cstrRecord.unit, "0:1");
  BOOST_CHECK_EQUAL(cstrRecord.comment, "SOC Planning ID");
  primary.writeRecord("WEIRD", 2, "m", "[0:1] SOC Planning ID");
  const auto weirdRecord = primary.parseRecord<std::string>("WEIRD");
  BOOST_CHECK_EQUAL(weirdRecord.unit, "m");
  BOOST_CHECK_EQUAL(weirdRecord.comment, "[0:1] SOC Planning ID");
}

BOOST_FIXTURE_TEST_CASE(header_read_test, Test::TemporarySifFile) {
  const auto header = this->header().readHeader();
  BOOST_CHECK_GT(header.size(), 0);
  BOOST_CHECK_EQUAL(header.size() % 80, 0);
  // TODO check contents
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
