// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(StringUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(split_test) {
  const std::string input = "\na\nb\nc\n\n";
  const std::vector<std::string> expected = {"", "a", "b", "c", "", ""};
  BOOST_TEST(String::split(input) == expected);
}

BOOST_AUTO_TEST_CASE(trim_test) {
  const std::string input = "\n\ta\n\tb\n c\n \n";
  const std::string expected = "a\n\tb\n c";
  BOOST_TEST(String::trim(input) == expected);
}

BOOST_AUTO_TEST_CASE(c_str_array_test) {
  std::vector<std::string> vec {"Fievre", "Chat", "Bebe", "Passe-miroir"};
  String::CStrArray arr(vec);
  for (std::size_t row = 0; row < vec.size(); ++row) {
    for (std::size_t letter = 0; letter < vec[row].length(); ++letter) {
      BOOST_TEST(vec[row].c_str()[letter] == arr.data()[row][letter]);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
