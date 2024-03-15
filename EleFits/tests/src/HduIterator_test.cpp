// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h" // Includes HduIterator

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduIterator_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(range_loop_over_all_hdus, Test::TemporaryMefFile)
{
  this->append_image_header("1");
  this->append_image_header("2");
  int count = 0;
  for (const auto& hdu : *this) {
    BOOST_TEST(hdu.matches(HduCategory::Image));
    count++;
  }
  BOOST_TEST(count == this->hdu_count());
}

BOOST_FIXTURE_TEST_CASE(range_loop_over_selected_hdus, Test::TemporaryMefFile)
{
  ColumnInfo<float> info {"COL", "", 1};
  std::vector<std::string> names {"", "BINTABLE1", "BINTABLE2", "IMAGE"};
  this->append_bintable_header(names[1], {}, info);
  this->append_bintable_header(names[2], {}, info);
  this->append_null_image<float, 2>(names[3], {}, {1, 1});

  int count = 0;
  std::vector<std::string> read_names;

  for (const auto& hdu : this->filter<ImageHdu>(HduCategory::Primary)) {
    const std::string name = hdu.read_name();
    BOOST_TEST(name == names[0]);
    read_names.push_back(name);
    BOOST_TEST(hdu.matches(HduCategory::Image));
    count++;
  }
  BOOST_TEST(count == 1);

  for (const auto& hdu : this->filter<BintableHdu>(HduCategory::Any)) {
    read_names.push_back(hdu.read_name());
    BOOST_TEST(hdu.matches(HduCategory::Bintable & HduCategory::Ext));
    count++;
  }
  BOOST_TEST(count == 3);

  for (const auto& hdu : this->filter<ImageHdu>(HduCategory::Ext)) {
    read_names.push_back(hdu.read_name());
    BOOST_TEST(hdu.matches(HduCategory::Image - HduCategory::Primary));
    count++;
  }
  BOOST_TEST(count == 4);

  BOOST_TEST(read_names == names);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
