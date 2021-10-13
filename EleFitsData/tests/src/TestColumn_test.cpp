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

#include "EleFitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(TestColumn_test, Test::RandomTable)

//-----------------------------------------------------------------------------

#define COLUMN_HAS_MORE_THAN_2_ROWS_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_column_has_more_than_2_rows_test) { \
    BOOST_TEST(getColumn<type>().rowCount() > 2); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(COLUMN_HAS_MORE_THAN_2_ROWS_TEST)

#define PUSH_BACK_NAME(type, unused) names.push_back(getColumn<type>().info.name);

BOOST_AUTO_TEST_CASE(names_are_all_different_test) {
  BOOST_TEST(std::tuple_size<decltype(columns)>::value == columnCount);
  std::vector<std::string> names;
  ELEFITS_FOREACH_COLUMN_TYPE(PUSH_BACK_NAME)
  BOOST_TEST(names.size() == columnCount);
  for (long lhs = 0; lhs < columnCount; ++lhs) {
    const auto& vlhs = names[lhs];
    for (long rhs = 0; rhs < lhs; ++rhs) {
      const auto& vrhs = names[rhs];
      BOOST_TEST(vlhs != vrhs);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
