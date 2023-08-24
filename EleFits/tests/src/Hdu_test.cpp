// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/Hdu.h"
#include "EleFits/MefFile.h"
#include "EleFits/SifFile.h"
#include "EleFitsData/TestRecord.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Hdu_test)

//-----------------------------------------------------------------------------

template <typename T>
void check_record_with_fallback_is_read_back(const Header& h, const std::string& keyword) {
  BOOST_TEST(not h.has(keyword));
  BOOST_CHECK_THROW(h.parse<T>(keyword), std::exception);
  const Record<T> fallback {keyword, Test::generate_random_value<T>(), "", "FALLBACK"};
  auto output = h.parse_or<T>(fallback);
  BOOST_TEST((output == fallback));
  const Record<T> input {keyword, Test::generate_random_value<T>(), "", "INPUT"};
  h.write(input);
  BOOST_TEST((input != fallback)); // At least the comments differ
  output = h.parse_or<T>(fallback);
  BOOST_TEST(output.keyword == input.keyword);
  BOOST_TEST(Test::approx(output.value, input.value));
  BOOST_TEST(output.unit == input.unit);
  BOOST_TEST(output.comment == input.comment);
}

template <>
void check_record_with_fallback_is_read_back<unsigned long>(const Header& h, const std::string& keyword) {
  // Wait for CFITSIO bug to be fixed
  (void)h;
  (void)keyword;
}

template <>
void check_record_with_fallback_is_read_back<unsigned long long>(const Header& h, const std::string& keyword) {
  // Wait for CFITSIO bug to be fixed
  (void)h;
  (void)keyword;
}

#define RECORD_WITH_FALLBACK_IS_READ_BACK_TEST(type, name) \
  BOOST_FIXTURE_TEST_CASE(name##_record_with_fallback_is_read_back_test, Test::TemporarySifFile) { \
    check_record_with_fallback_is_read_back<type>(this->header(), std::string(#name).substr(0, 8)); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_WITH_FALLBACK_IS_READ_BACK_TEST)

BOOST_FIXTURE_TEST_CASE(records_with_fallback_are_read_back_test, Test::TemporarySifFile) {
  Record<short> written("SHORT", 1);
  Record<long> fallback("LONG", 10);
  const auto& header = this->header();
  BOOST_TEST(not header.has(written.keyword));
  BOOST_TEST(not header.has(fallback.keyword));
  header.write(written);
  written.value++;
  fallback.value++;
  const auto output = header.parse_n_or(written, fallback);
  BOOST_TEST(std::get<0>(output).value == written.value - 1);
  BOOST_TEST(std::get<1>(output).value == fallback.value);
}

BOOST_FIXTURE_TEST_CASE(long_string_value_is_read_back_test, Test::TemporarySifFile) {
  const auto& h = this->header();
  const std::string short_str = "S";
  const std::string long_str =
      "This is probably one of the longest strings "
      "that I have ever written in a serious code.";
  BOOST_TEST(long_str.length() > FLEN_VALUE);
  h.write("SHORT", short_str);
  BOOST_TEST(not h.has("LONGSTRN"));
  h.write("LONG", long_str);
  const auto output = h.parse<std::string>("LONG");
  BOOST_TEST(h.has("LONGSTRN"));
  BOOST_TEST(output.value == long_str);
  BOOST_TEST(output.has_long_string_value());
}

void check_hierarch_keyword_is_read_back(const Header& h, const std::string& keyword) {
  BOOST_TEST(h.read_all().find("HIERARCH") == std::string::npos); // Not found
  const Record<int> record(keyword, 10);
  BOOST_TEST(record.has_long_keyword() == (keyword.length() > 8));
  h.write(record);
  BOOST_TEST(h.read_all().find("HIERARCH") != std::string::npos); // Found
  const auto output = h.parse<int>(keyword);
  BOOST_TEST(output.value == 10);
}

BOOST_FIXTURE_TEST_CASE(long_keyword_is_read_back_test, Test::TemporarySifFile) {
  check_hierarch_keyword_is_read_back(this->header(), "123456789");
}

BOOST_FIXTURE_TEST_CASE(keyword_with_space_is_read_back_test, Test::TemporarySifFile) {
  check_hierarch_keyword_is_read_back(this->header(), "A B");
}

BOOST_FIXTURE_TEST_CASE(keyword_with_symbol_is_read_back_test, Test::TemporarySifFile) {
  check_hierarch_keyword_is_read_back(this->header(), "1$");
}

BOOST_FIXTURE_TEST_CASE(hdu_is_renamed_test, Test::TemporaryMefFile) {
  const auto& h = this->append_image_header("A");
  BOOST_TEST(h.index() == 1);
  BOOST_TEST(h.read_name() == "A");
  h.update_name("B");
  BOOST_TEST(h.read_name() == "B");
  h.header().remove("EXTNAME");
  BOOST_TEST(h.read_name() == "");
}

BOOST_FIXTURE_TEST_CASE(c_str_record_is_read_back_as_string_record_test, Test::TemporarySifFile) {
  const auto& h = this->header();
  h.write("C_STR", "1");
  const auto output1 = h.parse<std::string>("C_STR");
  BOOST_TEST(output1.value == "1");
  h.write<RecordMode::UpdateExisting>("C_STR", "2");
  const auto output2 = h.parse<std::string>("C_STR");
  BOOST_TEST(output2.value == "2");
}

BOOST_FIXTURE_TEST_CASE(record_tuple_is_updated_and_read_back_test, Test::TemporarySifFile) {
  const auto& h = this->header();
  const Record<short> short_record {"SHORT", 1};
  const Record<long> long_record {"LONG", 1000};
  auto records = std::make_tuple(short_record, long_record);
  h.write_n(records);
  BOOST_TEST(h.parse<short>("SHORT") == 1);
  BOOST_TEST(h.parse<long>("LONG") == 1000);
  std::get<0>(records).value = 2;
  std::get<1>(records).value = 2000;
  h.write_n<RecordMode::UpdateExisting>(records);
  BOOST_TEST(h.parse<short>("SHORT") == 2);
  BOOST_TEST(h.parse<long>("LONG") == 2000);
}

BOOST_FIXTURE_TEST_CASE(vector_of_any_records_is_read_back_test, Test::TemporarySifFile) {
  const auto& h = this->header();
  std::vector<Record<VariantValue>> records;
  records.push_back({"STRING", std::string("WIDE")});
  records.push_back({"FLOAT", 3.14F});
  records.push_back({"INT", 666});
  h.write_n(records);
  auto parsed = h.parse_all();
  BOOST_TEST(parsed.as<std::string>("STRING").value == "WIDE");
  BOOST_TEST(parsed.as<int>("INT").value == 666);
  BOOST_CHECK_THROW(parsed.as<std::string>("INT"), std::exception);
}

BOOST_FIXTURE_TEST_CASE(subset_of_vector_of_any_records_is_read_back_test, Test::TemporarySifFile) {
  const auto& h = this->header();
  RecordSeq records(3);
  records.vector[0].assign(Record<std::string>("STRING", "WIDE"));
  records.vector[1].assign(Record<float>("FLOAT", 3.14F));
  records.vector[2].assign(Record<int>("INT", 666));
  h.write_n_in({"FLOAT", "INT"}, records);
  BOOST_CHECK_THROW(h.parse<VariantValue>("STRING"), std::exception);
  auto parsed = h.parse_n({"INT"});
  BOOST_TEST(parsed.as<int>("INT").value == 666);
  BOOST_CHECK_THROW(parsed.as<float>("FLOAT"), std::exception);
}

BOOST_FIXTURE_TEST_CASE(brackets_in_comment_are_read_back_test, Test::TemporaryMefFile) {
  const auto& primary = this->primary().header();
  primary.write("PLAN_ID", 1, "", "[0:1] SOC Planning ID");
  const auto int_record = primary.parse<int>("PLAN_ID");
  BOOST_TEST(int_record.unit == "0:1");
  BOOST_TEST(int_record.comment == "SOC Planning ID");
  primary.write("STRING", std::string("1"), "", "[0:1] SOC Planning ID");
  const auto string_record = primary.parse<std::string>("STRING");
  BOOST_TEST(string_record.unit == "0:1");
  BOOST_TEST(string_record.comment == "SOC Planning ID");
  primary.write("CSTR", "1", "", "[0:1] SOC Planning ID");
  const auto cstr_record = primary.parse<std::string>("CSTR");
  BOOST_TEST(cstr_record.unit == "0:1");
  BOOST_TEST(cstr_record.comment == "SOC Planning ID");
  primary.write("WEIRD", 2, "m", "[0:1] SOC Planning ID");
  const auto weird_record = primary.parse<std::string>("WEIRD");
  BOOST_TEST(weird_record.unit == "m");
  BOOST_TEST(weird_record.comment == "[0:1] SOC Planning ID");
}

BOOST_FIXTURE_TEST_CASE(comment_and_history_are_written, Test::TemporarySifFile) {
  const auto& header = this->header();
  const std::string comment = "BLUE";
  const std::string history = "BEAVER";
  header.write_comment(comment);
  header.write_history(history);
  const auto contents = header.read_all();
  BOOST_TEST((contents.find(comment) != std::string::npos));
  BOOST_TEST((contents.find(history) != std::string::npos));
}

BOOST_FIXTURE_TEST_CASE(full_header_is_read_as_string_test, Test::TemporarySifFile) {
  const auto header = this->header().read_all(); // TODO test with categories
  BOOST_TEST(header.size() > 0);
  BOOST_TEST(header.size() % 80 == 0);
  // TODO check contents
}

BOOST_FIXTURE_TEST_CASE(records_are_read_as_a_struct_test, Test::TemporarySifFile) {
  struct MyHeader {
    bool b;
    int i;
    float f;
    std::string s;
  };
  const auto& header = this->header();
  const MyHeader input {false, 1, 3.14F, "VAL"};
  header.write_n(
      Record<bool>("BOOL", input.b),
      Record<int>("INT", input.i),
      Record<float>("FLOAT", input.f),
      Record<std::string>("STRING", input.s));
  const auto output =
      header.parse_struct<MyHeader>(as<bool>("BOOL"), as<int>("INT"), as<float>("FLOAT"), as<std::string>("STRING"));
  BOOST_TEST(output.b == input.b);
  BOOST_TEST(output.i == input.i);
  BOOST_TEST(output.f == input.f);
  BOOST_TEST(output.s == input.s);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
