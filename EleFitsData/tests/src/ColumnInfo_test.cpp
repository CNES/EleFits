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

#include "EleFitsData/ColumnInfo.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ColumnInfo_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkElementCountPerRow() {
  constexpr long repeat = 17; // Not 1 ;)
  ColumnInfo<T> info {"COL", "unit", repeat};
  BOOST_TEST(info.repeatCount() == repeat);
  if (std::is_same<T, std::string>::value) {
    BOOST_TEST(info.elementCountPerEntry() == 1);
  } else {
    BOOST_TEST(info.elementCountPerEntry() == repeat);
  }
}

#define ELEMENT_COUNT_PER_ROW_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_element_count_per_row) { \
    checkElementCountPerRow<type>(#name); \
  }

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
