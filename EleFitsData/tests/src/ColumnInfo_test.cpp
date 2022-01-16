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
void checkElementCount() {
  constexpr long repeat = 17; // Not 1 ;)
  ColumnInfo<T> info {"COL", "unit", repeat};
  BOOST_TEST(info.repeatCount == repeat);
  if (std::is_same<T, std::string>::value) {
    BOOST_TEST(info.elementCount() == 1);
  } else {
    BOOST_TEST(info.elementCount() == repeat);
  }
}

#define ELEMENT_COUNT_TEST(T, name) \
  BOOST_AUTO_TEST_CASE(name##_element_count) { \
    checkElementCount<T>(); \
  }

template <long N>
void checkRepeatCountFromShape(const Position<N>& shape) {
  ColumnInfo<float, N> info("NAME", "unit", shape);
  BOOST_TEST(info.repeatCount == shapeSize(shape));
}

BOOST_AUTO_TEST_CASE(repeat_count_from_shape_test) {
  checkRepeatCountFromShape<-1>({1, 2, 3});
  checkRepeatCountFromShape<2>({3, 14});
  checkRepeatCountFromShape<3>({28, 6, 1989});
}

ELEFITS_FOREACH_COLUMN_TYPE(ELEMENT_COUNT_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
