// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRecord.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(TestRecord_test, Test::RandomHeader)

//-----------------------------------------------------------------------------

template <typename T>
void check_fields_are_all_set(const Test::RandomHeader& header)
{
  const auto& record = header.get_record<T>();
  BOOST_TEST(not record.keyword.empty());
  BOOST_TEST(not record.unit.empty());
  BOOST_TEST(not record.comment.empty());
}

template <typename T>
void check_fields_are_all_different(const Test::RandomHeader& header)
{
  const auto& record = header.get_record<T>();
  BOOST_TEST(record.keyword != record.unit);
  BOOST_TEST(record.unit != record.comment);
  BOOST_TEST(record.comment != record.keyword);
}

#define FIELDS_ARE_ALL_SET_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_fields_are_all_set_test) \
  { \
    check_fields_are_all_set<type>(*this); \
  }

ELEFITS_FOREACH_RECORD_TYPE(FIELDS_ARE_ALL_SET_TEST)

#define FIELDS_ARE_ALL_DIFFERENT_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_fields_are_all_different_test) \
  { \
    check_fields_are_all_different<type>(*this); \
  }

ELEFITS_FOREACH_RECORD_TYPE(FIELDS_ARE_ALL_DIFFERENT_TEST)

BOOST_AUTO_TEST_CASE(keywords_are_all_different_test)
{
  const auto v = all_record().vector;
  BOOST_TEST(v.size() == record_count);
  for (Linx::Index lhs = 0; lhs < record_count; ++lhs) {
    const auto& vlhs = v[lhs];
    for (Linx::Index rhs = 0; rhs < lhs; ++rhs) {
      const auto& vrhs = v[rhs];
      BOOST_TEST(vlhs.keyword != vrhs.keyword);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
