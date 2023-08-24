// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ColumnKey.h"
#include "EleFits/TestBintable.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(ColumnKey_test, Test::TestBintable<float>)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(from_resolved_index_test) {
  ColumnKey key(0);
  BOOST_TEST(key.index(columns) == 0);
  BOOST_TEST(key.name(columns) == scalar_column.info().name);
}

BOOST_AUTO_TEST_CASE(from_unresolved_index_test) {
  ColumnKey key(-1);
  BOOST_TEST(key.index(columns) == 1);
  BOOST_TEST(key.name(columns) == vector_column.info().name);
}

BOOST_AUTO_TEST_CASE(from_name_test) {
  ColumnKey key(scalar_column.info().name);
  BOOST_TEST(key.name(columns) == scalar_column.info().name);
  BOOST_TEST(key.index(columns) == 0);
}

void check_is_last(ColumnKey key, const BintableColumns& columns) {
  BOOST_TEST(key.index(columns) == 1);
}

BOOST_AUTO_TEST_CASE(implicit_cast_test) {
  check_is_last(1, columns);
  check_is_last(-1, columns);
  check_is_last(last_column.info().name, columns);
  check_is_last(last_column.info().name.c_str(), columns);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
