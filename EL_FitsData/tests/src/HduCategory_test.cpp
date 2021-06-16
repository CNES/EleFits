/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "EL_FitsData/HduCategory.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduCategory_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(category_ordering_test) {
  BOOST_TEST(isInstance(HduCategory::MetadataPrimary, HduCategory::Primary));
  BOOST_TEST(not isInstance(HduCategory::Primary, HduCategory::MetadataPrimary));
  BOOST_TEST(isInstance(HduCategory::Primary, HduCategory::Image));
  BOOST_TEST(not isInstance(HduCategory::Primary, HduCategory::ImageExt));
  BOOST_TEST(not isInstance(HduCategory::Image, HduCategory::Primary));
  BOOST_TEST(isInstance(HduCategory::Image, HduCategory::Any));
  BOOST_TEST(isInstance(HduCategory::Bintable, HduCategory::Any));
  BOOST_TEST(isInstance(HduCategory::FloatImage, HduCategory::Image));
  BOOST_TEST(isInstance(HduCategory::FloatImage & HduCategory::Ext, HduCategory::ImageExt));
}

BOOST_AUTO_TEST_CASE(filtering_test) {
  BOOST_TEST((+HduCategory::Image).accepts(HduCategory::ImageExt));
  BOOST_TEST(not(+HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((-HduCategory::Primary).accepts(HduCategory::ImageExt));
  BOOST_TEST((HduCategory::Image - HduCategory::Primary).accepts(HduCategory::ImageExt));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
