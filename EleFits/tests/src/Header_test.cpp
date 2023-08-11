// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/Hdu.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(Header_test, Test::TemporarySifFile)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(long_string_record_is_read_back_test) {

  /* From the FITS standard */
  const std::string keyword = "STRKEY";
  const std::string long_value = "This keyword value is continued over multiple keyword records.";
  const std::string long_comment = "The comment field for this keyword is also continued over multiple records";
  const auto& h = this->header();

  /* Long value, no comment */
  const Record<std::string> long_value_record(keyword, long_value);
  h.write(long_value_record);
  auto output = h.parse<std::string>(keyword);
  BOOST_TEST(output == long_value_record);

  /* Long comment, no value */
  const Record<std::string> long_comment_record(keyword, "", "", long_comment);
  h.write<RecordMode::UpdateExisting>(long_comment_record);
  output = h.parse<std::string>(keyword);
  BOOST_TEST(output.comment != long_comment); // CFITSIO bug/limitation: comment is truncated
  BOOST_TEST(output.value == "");

  /* Long value and comment */
  const Record<std::string> long_value_and_comment_record(keyword, long_value, "", long_comment);
  h.write<RecordMode::UpdateExisting>(long_value_and_comment_record);
  output = h.parse<std::string>(keyword);
  BOOST_TEST(output.comment != long_comment); // CFITSIO bug/limitation: comment is truncated
  BOOST_TEST(output.value == long_value);
}

BOOST_AUTO_TEST_CASE(long_comment_hierarch_record_is_read_back_test) {

  /* From the FITS standard */
  const std::string long_keyword = "123456789";
  const int value = 10;
  const std::string long_comment =
      "Manuel is trying to crash EleFits with a very very very long comment in a hierarch keyword!";
  const auto& h = this->header();

  /* Long value, no comment */
  const Record<int> long_comment_hierarch_record(long_keyword, value, "", long_comment);
  h.write(long_comment_hierarch_record);
  BOOST_TEST(h.has("HIERARCH *")); // TODO * is not officially supported
  auto output = h.parse<int>(long_keyword);
  BOOST_TEST(output.comment != long_comment); // Nominal: comment is truncated for non-string keywords
  BOOST_TEST(output.value == value);
}

BOOST_AUTO_TEST_CASE(keyword_error_test) {
  const std::string keyword = "TEST";
  KeywordExistsError kee(keyword);
  KeywordNotFoundError knfe(keyword);
  BOOST_TEST(kee.keyword == keyword);
  BOOST_TEST(knfe.keyword == keyword);
}

BOOST_AUTO_TEST_CASE(syntax_test) {

  /* Setup */
  const auto& h = header();
  const Record<int> i("I", 1);
  const Record<float> f("F", 3.14);
  const auto t = std::make_tuple(i, f);
  const auto v = RecordSeq {i, f};
  struct S {
    int i;
    float f;
  };

  /* Single write */
  h.write("I", 0);
  h.write(i);
  h.write<RecordMode::CreateNew>("I", 0);
  h.write<RecordMode::CreateNew>("I", 0);

  /* Heterogeneous write */
  h.write_seq(i, f);
  h.write_seq(t);
  h.write_seq_in({"I"}, i, f);
  h.write_seq_in({"F"}, t);
  h.write_seq<RecordMode::CreateNew>(i, f);
  h.write_seq<RecordMode::CreateNew>(t);

  /* Homogeneous write */
  h.write_seq(v.vector);
  h.write_seq_in({"I"}, v.vector);
  h.write_seq<RecordMode::CreateNew>(v.vector);

  /* Global read */
  h.read_all(~KeywordCategory::Comment);
  h.parse_all(~KeywordCategory::Comment);

  /* Single read */
  h.parse<int>(i.keyword);
  h.parse_or<int>(i.keyword, 0);
  h.parse_or(i);

  /* Heterogeneous read */
  h.parse_seq(as<int>("I"), as<float>("F"));
  h.parse_seq_or(std::make_tuple(Record<int>("I", 0), Record<float>("F", 3.14)));
  const auto [bound_i, bound_f] = h.parse_struct<S>(as<int>("I"), as<float>("F")); // Structured binding
  BOOST_TEST(bound_i == i);
  BOOST_TEST(bound_f == f);

  /* Homogeneous read */
  h.parse_seq<VariantValue>({"I", "F"});
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
