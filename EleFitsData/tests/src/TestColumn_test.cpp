// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(TestColumn_test, Test::RandomTable)

//-----------------------------------------------------------------------------

template <typename T>
void check_random_vector_column_size(Linx::Index repeat_count, Linx::Index row_count)
{
  Test::RandomVectorColumn<T> column(repeat_count, row_count);
  BOOST_TEST(column.info().repeat_count() == repeat_count);
  BOOST_TEST(column.row_count() == row_count);
  if (std::is_same<T, std::string>::value) {
    BOOST_TEST(column.size() == row_count);
  } else {
    BOOST_TEST(column.size() == repeat_count * row_count);
  }
}

#define RANDOM_VECTOR_COLUMN_SIZE_TEST(T, name) \
  BOOST_AUTO_TEST_CASE(name##_random_vector_column_size_test) \
  { \
    check_random_vector_column_size<T>(4, 5); \
  }

#define COLUMN_HAS_MORE_THAN_2_ROWS_TEST(T, name) \
  BOOST_AUTO_TEST_CASE(name##_column_has_more_than_2_rows_test) \
  { \
    BOOST_TEST(get_column<T>().row_count() > 2); \
  }

ELEFITS_FOREACH_COLUMN_TYPE(COLUMN_HAS_MORE_THAN_2_ROWS_TEST)

#define PUSH_BACK_NAME(T, _) names.push_back(get_column<T>().info().name);

BOOST_AUTO_TEST_CASE(names_are_all_different_test)
{
  BOOST_TEST(std::tuple_size<decltype(columns)>::value == column_count);
  std::vector<std::string> names;
  ELEFITS_FOREACH_COLUMN_TYPE(PUSH_BACK_NAME)
  BOOST_TEST(names.size() == column_count);
  for (Linx::Index lhs = 0; lhs < column_count; ++lhs) {
    const auto& vlhs = names[lhs];
    for (Linx::Index rhs = 0; rhs < lhs; ++rhs) {
      const auto& vrhs = names[rhs];
      BOOST_TEST(vlhs != vrhs);
    }
  }
}

BOOST_FIXTURE_TEST_CASE(small_table_size_test, Test::SmallTable)
{
  const auto rows = num_col.row_count();
  BOOST_TEST(rows > 1);
  BOOST_TEST(radec_col.row_count() == rows);
  BOOST_TEST(name_col.row_count() == rows);
  BOOST_TEST(dist_mag_col.row_count() == rows);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
