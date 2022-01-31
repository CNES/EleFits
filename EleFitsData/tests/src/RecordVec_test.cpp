// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/RecordVec.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(RecordVec_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(records_are_found_by_their_keyword_test) {
  RecordVec<int> records(3);
  for (std::size_t i = 0; i < records.vector.size(); ++i) {
    records.vector[i].assign(std::to_string(i), int(i));
  }
  BOOST_TEST(records["1"].value == 1);
  BOOST_TEST(records["2"].value == 2);
  BOOST_CHECK_THROW(records["OOPS!"], std::exception);
}

BOOST_AUTO_TEST_CASE(records_are_cast_while_found_by_their_keyword_test) {
  RecordVec<double> records(1);
  records.vector[0].assign("PI", 3.14);
  auto pi_record = records.as<int>("PI");
  BOOST_TEST(pi_record.value == 3);
  int pi = records.as<int>("PI");
  BOOST_TEST(pi == 3);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
