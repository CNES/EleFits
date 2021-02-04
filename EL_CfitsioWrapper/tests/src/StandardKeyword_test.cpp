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

#include <boost/test/unit_test.hpp>

#include "EL_CfitsioWrapper/StandardKeyword.h"

using namespace Euclid::Cfitsio;
using Category = Header::StandardKeyword::Category;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(StandardKeyword_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(standard_keyword_matching_test) {
  BOOST_CHECK(Header::StandardKeyword::matches("KEY", "KEY"));
  BOOST_CHECK(not Header::StandardKeyword::matches("KEY", "KEYn"));
  BOOST_CHECK(Header::StandardKeyword::matches("KEYn", "KEYn"));
  BOOST_CHECK(Header::StandardKeyword::matches("KEY123", "KEYn"));
  BOOST_CHECK(not Header::StandardKeyword::matches("KEYn", "KEY123"));
  BOOST_CHECK(not Header::StandardKeyword::matches("KEYWORD", "KEYn"));
}

BOOST_AUTO_TEST_CASE(mandatory_masking_test) {
  BOOST_CHECK(Category::Mandatory & Category::Mandatory);
  BOOST_CHECK(Category::Mandatory & Category::All);
  BOOST_CHECK(not(Category::Mandatory & (Category::Reserved | Category::Comment | Category::User)));
  BOOST_CHECK(not(Category::Mandatory & (Category::All & ~Category::Mandatory)));
}

BOOST_AUTO_TEST_CASE(reserved_masking_test) {
  BOOST_CHECK(Category::Reserved & Category::Reserved);
  BOOST_CHECK(Category::Reserved & Category::All);
  BOOST_CHECK(not(Category::Reserved & (Category::Mandatory | Category::Comment | Category::User)));
  BOOST_CHECK(not(Category::Reserved & (Category::All & ~Category::Reserved)));
}

BOOST_AUTO_TEST_CASE(comment_masking_test) {
  BOOST_CHECK(Category::Comment & Category::Comment);
  BOOST_CHECK(Category::Comment & Category::All);
  BOOST_CHECK(not(Category::Comment & (Category::Mandatory | Category::Reserved | Category::User)));
  BOOST_CHECK(not(Category::Comment & (Category::All & ~Category::Comment)));
}

BOOST_AUTO_TEST_CASE(user_masking_test) {
  BOOST_CHECK(Category::User & Category::User);
  BOOST_CHECK(Category::User & Category::All);
  BOOST_CHECK(not(Category::User & (Category::Mandatory | Category::Reserved | Category::Comment)));
  BOOST_CHECK(not(Category::User & (Category::All & ~Category::User)));
}

BOOST_AUTO_TEST_CASE(mandatory_categorization_test) {
  const std::string mandatory = "SIMPLE";
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(mandatory, Category::Mandatory));
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(mandatory, Category::Mandatory | Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(mandatory, Category::Reserved));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(mandatory, Category::Comment));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(mandatory, Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(
      mandatory,
      Category::Reserved | Category::Comment | Category::User));
}

BOOST_AUTO_TEST_CASE(reserved_categorization_test) {
  const std::string reserved = "TFORM1";
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(reserved, Category::Reserved));
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(reserved, Category::Reserved | Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(reserved, Category::Mandatory));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(reserved, Category::Comment));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(reserved, Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(
      reserved,
      Category::Mandatory | Category::Comment | Category::User));
}

BOOST_AUTO_TEST_CASE(comment_categorization_test) {
  const std::string comment = "COMMENT";
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(comment, Category::Comment));
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(comment, Category::Comment | Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(comment, Category::Mandatory));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(comment, Category::Reserved));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(comment, Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(
      comment,
      Category::Reserved | Category::Mandatory | Category::User));
}

BOOST_AUTO_TEST_CASE(user_categorization_test) {
  const std::string user = "MINE";
  BOOST_CHECK(Header::StandardKeyword::belongsCategories(user, Category::User));
  BOOST_CHECK(not Header::StandardKeyword::belongsCategories(
      user,
      Category::Mandatory | Category::Reserved | Category::Comment));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
