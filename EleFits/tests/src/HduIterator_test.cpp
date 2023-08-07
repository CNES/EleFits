// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h" // Includes HduIterator

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduIterator_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(range_loop_over_all_hdus, Test::TemporaryMefFile) {
  this->appendImageHeader("1");
  this->appendImageHeader("2");
  int count = 0;
  for (const auto& hdu : *this) {
    BOOST_TEST(hdu.matches(HduCategory::Image));
    count++;
  }
  BOOST_TEST(count == this->hduCount());
}

BOOST_FIXTURE_TEST_CASE(range_loop_over_selected_hdus, Test::TemporaryMefFile) {
  ColumnInfo<float> info {"COL", "", 1};
  std::vector<std::string> names {"", "BINTABLE1", "BINTABLE2", "IMAGE"};
  this->appendBintableHeader(names[1], {}, info);
  this->appendBintableHeader(names[2], {}, info);
  this->appendNullImage<float, 2>(names[3], {}, {1, 1});

  int count = 0;
  std::vector<std::string> readNames;

  for (const auto& hdu : this->filter<ImageHdu>(HduCategory::Primary)) {
    const std::string name = hdu.readName();
    BOOST_TEST(name == names[0]);
    readNames.push_back(name);
    BOOST_TEST(hdu.matches(HduCategory::Image));
    count++;
  }
  BOOST_TEST(count == 1);

  for (const auto& hdu : this->filter<BintableHdu>(HduCategory::Any)) {
    readNames.push_back(hdu.readName());
    BOOST_TEST(hdu.matches(HduCategory::Bintable & HduCategory::Ext));
    count++;
  }
  BOOST_TEST(count == 3);

  for (const auto& hdu : this->filter<ImageHdu>(HduCategory::Ext)) {
    readNames.push_back(hdu.readName());
    BOOST_TEST(hdu.matches(HduCategory::Image - HduCategory::Primary));
    count++;
  }
  BOOST_TEST(count == 4);

  BOOST_TEST(readNames == names);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
