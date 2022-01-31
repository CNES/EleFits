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
  h.writeSeq(i, f);
  h.writeSeq(t);
  h.writeSeqIn({"I"}, i, f);
  h.writeSeqIn({"F"}, t);
  h.writeSeq<RecordMode::CreateNew>(i, f);
  h.writeSeq<RecordMode::CreateNew>(t);

  /* Homogeneous write */
  h.writeSeq(v.vector);
  h.writeSeqIn({"I"}, v.vector);
  h.writeSeq<RecordMode::CreateNew>(v.vector);

  /* Global read */
  h.readAll(~KeywordCategory::Comment);
  h.parseAll(~KeywordCategory::Comment);

  /* Single read */
  h.parse<int>(i.keyword);
  h.parseOr<int>(i.keyword, 0);
  h.parseOr(i);

  /* Heterogeneous read */
  h.parseSeq(as<int>("I"), as<float>("F"));
  h.parseSeqOr(std::make_tuple(Record<int>("I", 0), Record<float>("F", 3.14)));
  h.parseStruct<S>(as<int>("I"), as<float>("F"));

  /* Homogeneous read */
  h.parseSeq<VariantValue>({"I", "F"});
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
