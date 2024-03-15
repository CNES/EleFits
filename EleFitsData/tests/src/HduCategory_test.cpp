// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/HduCategory.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduCategory_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(type_test)
{
  BOOST_TEST((HduCategory::Any.type() == HduCategory::Any));
  BOOST_TEST((HduCategory::Primary.type() == HduCategory::Image));
  BOOST_TEST((HduCategory::Ext.type() == HduCategory::Any));
  BOOST_TEST((HduCategory::Image.type() == HduCategory::Image));
  BOOST_TEST((HduCategory::Bintable.type() == HduCategory::Bintable));
}

BOOST_AUTO_TEST_CASE(operators_test)
{
  BOOST_TEST((HduCategory::Primary == HduCategory::Primary));
  BOOST_TEST((HduCategory::Bintable == ~HduCategory::Primary));
  BOOST_TEST(((HduCategory::Image & HduCategory::Ext) == HduCategory::ImageExt));
  BOOST_TEST(((HduCategory::Image & ~HduCategory::Ext) == HduCategory::Primary));
  BOOST_TEST(((HduCategory::Primary | HduCategory::ImageExt) == HduCategory::Image));
  BOOST_TEST(((HduCategory::MetadataPrimary & HduCategory::Primary) == HduCategory::MetadataPrimary));
  BOOST_TEST(((HduCategory::Primary & HduCategory::MetadataPrimary) == HduCategory::MetadataPrimary));
  BOOST_TEST(((HduCategory::Primary << HduCategory::Ext) == HduCategory::ImageExt));
  BOOST_TEST(((HduCategory::Image << HduCategory::Ext) == HduCategory::ImageExt));
  BOOST_TEST(
      (((HduCategory::Untouched & HduCategory::Primary) << HduCategory::Touched) ==
       (HduCategory::Touched & HduCategory::Primary)));
  BOOST_TEST((HduCategory::MetadataPrimary != HduCategory::Primary));
}

BOOST_AUTO_TEST_CASE(category_ordering_test)
{
  BOOST_TEST(HduCategory::MetadataPrimary.isInstance(HduCategory::Primary));
  BOOST_TEST(not HduCategory::Primary.isInstance(HduCategory::MetadataPrimary));
  BOOST_TEST(HduCategory::Primary.isInstance(HduCategory::Image));
  BOOST_TEST(not HduCategory::Primary.isInstance(HduCategory::ImageExt));
  BOOST_TEST(not HduCategory::Image.isInstance(HduCategory::Primary));
  BOOST_TEST(HduCategory::Image.isInstance(HduCategory::Any));
  BOOST_TEST(HduCategory::Bintable.isInstance(HduCategory::Any));
  BOOST_TEST(HduCategory::FloatImage.isInstance(HduCategory::Image));
  BOOST_TEST((HduCategory::FloatImage & HduCategory::Ext).isInstance(HduCategory::ImageExt));
}

BOOST_AUTO_TEST_CASE(filtering_test)
{
  BOOST_TEST((+HduCategory::Image).accepts(HduCategory::ImageExt));
  BOOST_TEST(not(+HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((-HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((HduCategory::Image - HduCategory::Primary).accepts(HduCategory::ImageExt));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
