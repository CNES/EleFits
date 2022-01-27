/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFitsData/HduCategory.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduCategory_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operators_test) {
  BOOST_TEST((HduCategory::Primary == HduCategory::Primary));
  BOOST_TEST((HduCategory::Bintable == ~HduCategory::Primary));
  BOOST_TEST(((HduCategory::Image & HduCategory::Ext) == HduCategory::ImageExt));
  BOOST_TEST(((HduCategory::Image & ~HduCategory::Ext) == HduCategory::Primary));
  BOOST_TEST(((HduCategory::Primary | HduCategory::ImageExt) == HduCategory::Image));
  BOOST_TEST(((HduCategory::MetadataPrimary & HduCategory::Primary) == HduCategory::MetadataPrimary));
  BOOST_TEST(((HduCategory::Primary & HduCategory::MetadataPrimary) == HduCategory::MetadataPrimary));
  BOOST_TEST((HduCategory::MetadataPrimary != HduCategory::Primary));
}

BOOST_AUTO_TEST_CASE(category_ordering_test) {
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

BOOST_AUTO_TEST_CASE(filtering_test) {
  BOOST_TEST((+HduCategory::Image).accepts(HduCategory::ImageExt));
  BOOST_TEST(not(+HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((-HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((HduCategory::Image - HduCategory::Primary).accepts(HduCategory::ImageExt));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
