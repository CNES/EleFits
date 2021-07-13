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

#include "EL_FitsData/StandardKeyword.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(StandardKeyword_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(standard_keyword_matching_test) {
  BOOST_TEST(StandardKeyword::matches("KEY", "KEY"));
  BOOST_TEST(not StandardKeyword::matches("KEY", "KEYn"));
  BOOST_TEST(StandardKeyword::matches("KEYn", "KEYn"));
  BOOST_TEST(StandardKeyword::matches("KEY123", "KEYn"));
  BOOST_TEST(not StandardKeyword::matches("KEYn", "KEY123"));
  BOOST_TEST(not StandardKeyword::matches("KEYWORD", "KEYn"));
}

BOOST_AUTO_TEST_CASE(mandatory_categorization_test) {
  const std::string mandatory = "SIMPLE";
  BOOST_TEST(StandardKeyword::belongsCategories(mandatory, KeywordCategory::Mandatory));
  BOOST_TEST(StandardKeyword::belongsCategories(mandatory, KeywordCategory::Mandatory | KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::Reserved));
  BOOST_TEST(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::Comment));
  BOOST_TEST(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(
      mandatory,
      KeywordCategory::Reserved | KeywordCategory::Comment | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(reserved_categorization_test) {
  const std::string reserved = "TFORM1";
  BOOST_TEST(StandardKeyword::belongsCategories(reserved, KeywordCategory::Reserved));
  BOOST_TEST(StandardKeyword::belongsCategories(reserved, KeywordCategory::Reserved | KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(reserved, KeywordCategory::Mandatory));
  BOOST_TEST(not StandardKeyword::belongsCategories(reserved, KeywordCategory::Comment));
  BOOST_TEST(not StandardKeyword::belongsCategories(reserved, KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(
      reserved,
      KeywordCategory::Mandatory | KeywordCategory::Comment | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(comment_categorization_test) {
  const std::string comment = "COMMENT";
  BOOST_TEST(StandardKeyword::belongsCategories(comment, KeywordCategory::Comment));
  BOOST_TEST(StandardKeyword::belongsCategories(comment, KeywordCategory::Comment | KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(comment, KeywordCategory::Mandatory));
  BOOST_TEST(not StandardKeyword::belongsCategories(comment, KeywordCategory::Reserved));
  BOOST_TEST(not StandardKeyword::belongsCategories(comment, KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(
      comment,
      KeywordCategory::Reserved | KeywordCategory::Mandatory | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(user_categorization_test) {
  const std::string user = "MINE";
  BOOST_TEST(StandardKeyword::belongsCategories(user, KeywordCategory::User));
  BOOST_TEST(not StandardKeyword::belongsCategories(
      user,
      KeywordCategory::Mandatory | KeywordCategory::Reserved | KeywordCategory::Comment));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
