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
  BOOST_CHECK(StandardKeyword::matches("KEY", "KEY"));
  BOOST_CHECK(not StandardKeyword::matches("KEY", "KEYn"));
  BOOST_CHECK(StandardKeyword::matches("KEYn", "KEYn"));
  BOOST_CHECK(StandardKeyword::matches("KEY123", "KEYn"));
  BOOST_CHECK(not StandardKeyword::matches("KEYn", "KEY123"));
  BOOST_CHECK(not StandardKeyword::matches("KEYWORD", "KEYn"));
}

// BOOST_AUTO_TEST_CASE(mandatory_masking_test) {
//   BOOST_CHECK(KeywordCategory::Mandatory & KeywordCategory::Mandatory);
//   BOOST_CHECK(KeywordCategory::Mandatory & KeywordCategory::All);
//   BOOST_CHECK(
//       not(KeywordCategory::Mandatory & (KeywordCategory::Reserved | KeywordCategory::Comment | KeywordCategory::User)));
//   BOOST_CHECK(not(KeywordCategory::Mandatory & (KeywordCategory::All & ~KeywordCategory::Mandatory)));
// }

// BOOST_AUTO_TEST_CASE(reserved_masking_test) {
//   BOOST_CHECK(KeywordCategory::Reserved & KeywordCategory::Reserved);
//   BOOST_CHECK(KeywordCategory::Reserved & KeywordCategory::All);
//   BOOST_CHECK(
//       not(KeywordCategory::Reserved & (KeywordCategory::Mandatory | KeywordCategory::Comment | KeywordCategory::User)));
//   BOOST_CHECK(not(KeywordCategory::Reserved & (KeywordCategory::All & ~KeywordCategory::Reserved)));
// }

// BOOST_AUTO_TEST_CASE(comment_masking_test) {
//   BOOST_CHECK(KeywordCategory::Comment & KeywordCategory::Comment);
//   BOOST_CHECK(KeywordCategory::Comment & KeywordCategory::All);
//   BOOST_CHECK(
//       not(KeywordCategory::Comment & (KeywordCategory::Mandatory | KeywordCategory::Reserved | KeywordCategory::User)));
//   BOOST_CHECK(not(KeywordCategory::Comment & (KeywordCategory::All & ~KeywordCategory::Comment)));
// }

// BOOST_AUTO_TEST_CASE(user_masking_test) {
//   BOOST_CHECK(KeywordCategory::User & KeywordCategory::User);
//   BOOST_CHECK(KeywordCategory::User & KeywordCategory::All);
//   BOOST_CHECK(
//       not(KeywordCategory::User & (KeywordCategory::Mandatory | KeywordCategory::Reserved | KeywordCategory::Comment)));
//   BOOST_CHECK(not(KeywordCategory::User & (KeywordCategory::All & ~KeywordCategory::User)));
// }

BOOST_AUTO_TEST_CASE(mandatory_categorization_test) {
  const std::string mandatory = "SIMPLE";
  BOOST_CHECK(StandardKeyword::belongsCategories(mandatory, KeywordCategory::Mandatory));
  BOOST_CHECK(StandardKeyword::belongsCategories(mandatory, KeywordCategory::Mandatory | KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::Reserved));
  BOOST_CHECK(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::Comment));
  BOOST_CHECK(not StandardKeyword::belongsCategories(mandatory, KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(
      mandatory,
      KeywordCategory::Reserved | KeywordCategory::Comment | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(reserved_categorization_test) {
  const std::string reserved = "TFORM1";
  BOOST_CHECK(StandardKeyword::belongsCategories(reserved, KeywordCategory::Reserved));
  BOOST_CHECK(StandardKeyword::belongsCategories(reserved, KeywordCategory::Reserved | KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(reserved, KeywordCategory::Mandatory));
  BOOST_CHECK(not StandardKeyword::belongsCategories(reserved, KeywordCategory::Comment));
  BOOST_CHECK(not StandardKeyword::belongsCategories(reserved, KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(
      reserved,
      KeywordCategory::Mandatory | KeywordCategory::Comment | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(comment_categorization_test) {
  const std::string comment = "COMMENT";
  BOOST_CHECK(StandardKeyword::belongsCategories(comment, KeywordCategory::Comment));
  BOOST_CHECK(StandardKeyword::belongsCategories(comment, KeywordCategory::Comment | KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(comment, KeywordCategory::Mandatory));
  BOOST_CHECK(not StandardKeyword::belongsCategories(comment, KeywordCategory::Reserved));
  BOOST_CHECK(not StandardKeyword::belongsCategories(comment, KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(
      comment,
      KeywordCategory::Reserved | KeywordCategory::Mandatory | KeywordCategory::User));
}

BOOST_AUTO_TEST_CASE(user_categorization_test) {
  const std::string user = "MINE";
  BOOST_CHECK(StandardKeyword::belongsCategories(user, KeywordCategory::User));
  BOOST_CHECK(not StandardKeyword::belongsCategories(
      user,
      KeywordCategory::Mandatory | KeywordCategory::Reserved | KeywordCategory::Comment));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
