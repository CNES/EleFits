// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
  BOOST_TEST(info.repeat_count() == repeat);
  if (std::is_same<T, std::string>::value) {
    BOOST_TEST(info.element_count() == 1);
  } else {
    BOOST_TEST(info.element_count() == repeat);
  }
}

#define ELEMENT_COUNT_TEST(T, name) \
  BOOST_AUTO_TEST_CASE(name##_element_count) { \
    checkElementCount<T>(); \
  }

template <long N>
void checkRepeatCountFromShape(const Position<N>& shape) {
  ColumnInfo<float, N> info("NAME", "unit", shape);
  BOOST_TEST(info.repeat_count() == shapeSize(shape));
}

BOOST_AUTO_TEST_CASE(repeat_count_from_shape_test) {
  checkRepeatCountFromShape<-1>({1, 2, 3});
  checkRepeatCountFromShape<2>({3, 14});
  checkRepeatCountFromShape<3>({28, 6, 1989});
}

BOOST_AUTO_TEST_CASE(make_column_info_test) {
  auto string_info = make_column_info<std::string>("String", "", 6);
  BOOST_TEST((string_info.shape == Position<1> {6}));
  auto scalar_info = make_column_info<int>("Scalar");
  BOOST_TEST((scalar_info.shape == Position<1> {1}));
  auto vector_info = make_column_info<int>("Vector", "", 3);
  BOOST_TEST((vector_info.shape == Position<1> {3}));
  auto multidim_info = make_column_info<int>("Multidim", "", 3, 2);
  BOOST_TEST((multidim_info.shape == Position<2> {3, 2}));
}

ELEFITS_FOREACH_COLUMN_TYPE(ELEMENT_COUNT_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
