// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Column.h"
#include "EleFitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Column_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(column_data_can_be_shared_test)
{
  std::vector<int> input {1, 2, 3};
  PtrColumn<int> column({"SHARED", "", 1}, input.size(), input.data());
  BOOST_TEST(column.data()[1] == 2);
  input[1] = 4;
  BOOST_TEST(column.data()[1] == 4);
}

BOOST_AUTO_TEST_CASE(column_data_can_be_moved_test)
{
  std::vector<int> input {4, 5, 6};
  const auto data = input.data();
  VecColumn<int> column({"DATA", "", 1}, std::move(input));
  BOOST_TEST(column.data() == data);
  BOOST_TEST(column.container()[1] == 5);
  BOOST_TEST(input.size() == 0);
  column.move_to(input);
  BOOST_TEST(input.data() == data);
  BOOST_TEST(input[1] == 5);
  BOOST_TEST(column.container().size() == 0);
  BOOST_TEST(column.size() == 0);
}

BOOST_AUTO_TEST_CASE(subscript_bounds_test)
{
  const Linx::Index row_count = 10;
  const Linx::Index repeat_count = 3;
  Test::RandomVectorColumn<int> column(repeat_count, row_count);
  column.at(1, -1) = 1;
  BOOST_TEST(column.at(1, -1) == 1);
  const auto& vec = column.container();
  BOOST_TEST(column.at(0) == vec[0]);
  BOOST_TEST(column.at(-1) == vec[(row_count - 1) * repeat_count]);
  BOOST_TEST(column.at(-row_count) == vec[0]);
  BOOST_TEST(column.at(0, -1) == vec[repeat_count - 1]);
  BOOST_TEST(column.at(-1, -1) == vec[row_count * repeat_count - 1]);
  BOOST_CHECK_THROW(column.at(row_count), FitsError);
  BOOST_CHECK_THROW(column.at(-1 - row_count), FitsError);
  BOOST_CHECK_THROW(column.at(0, repeat_count), FitsError);
  BOOST_CHECK_THROW(column.at(0, -1 - repeat_count), FitsError);
}

BOOST_AUTO_TEST_CASE(numeric_column_elementcount_is_rowcount_times_repeatcount_test)
{
  constexpr int row_count = 17;
  constexpr int repeat_count = 7;
  // FIXME Breaks with int instead of Linx::Index:
  // CTor overload forwards to vector and creates 17 values instead of 17 * 7

  /* VecColumn */
  VecColumn<float> vec_column({"STR", "", repeat_count}, row_count);
  BOOST_TEST(vec_column.info().repeat_count() == repeat_count);
  BOOST_TEST(vec_column.row_count() == row_count);
  BOOST_TEST(vec_column.size() == row_count * repeat_count);

  /* PtrColumn */
  PtrColumn<float> ptr_column({"STR", "", repeat_count}, row_count, vec_column.data());
  BOOST_TEST(ptr_column.info().repeat_count() == repeat_count);
  BOOST_TEST(ptr_column.row_count() == row_count);
  BOOST_TEST(ptr_column.size() == row_count * repeat_count);

  /* Constant VecColumn */
  const VecColumn<float> c_vec_column({"STR", "", repeat_count}, row_count);
  BOOST_TEST(c_vec_column.info().repeat_count() == repeat_count);
  BOOST_TEST(c_vec_column.row_count() == row_count);
  BOOST_TEST(c_vec_column.size() == row_count * repeat_count);

  /* Constant PtrColumn */
  const PtrColumn<const float> c_ptr_olumn({"STR", "", repeat_count}, row_count, c_vec_column.data());
  BOOST_TEST(c_ptr_olumn.info().repeat_count() == repeat_count);
  BOOST_TEST(c_ptr_olumn.row_count() == row_count);
  BOOST_TEST(c_ptr_olumn.size() == row_count * repeat_count);
}

BOOST_AUTO_TEST_CASE(string_column_elementcount_is_rowcount_test)
{
  constexpr Linx::Index row_count = 17;
  constexpr Linx::Index repeat_count = 7;

  /* VecColumn */
  VecColumn<std::string> vec_column({"STR", "", repeat_count}, row_count);
  BOOST_TEST(vec_column.info().repeat_count() == repeat_count);
  BOOST_TEST(vec_column.row_count() == row_count);
  BOOST_TEST(vec_column.size() == row_count);

  /* PtrColumn */
  PtrColumn<std::string> ptr_column({"STR", "", repeat_count}, row_count, vec_column.data());
  BOOST_TEST(ptr_column.info().repeat_count() == repeat_count);
  BOOST_TEST(ptr_column.row_count() == row_count);
  BOOST_TEST(ptr_column.size() == row_count);

  /* Constant VecColumn */
  const VecColumn<std::string> c_vec_column({"STR", "", repeat_count}, row_count);
  BOOST_TEST(c_vec_column.info().repeat_count() == repeat_count);
  BOOST_TEST(c_vec_column.row_count() == row_count);
  BOOST_TEST(c_vec_column.size() == row_count);

  /* Constant PtrColumn */
  const PtrColumn<const std::string> c_ptr_olumn({"STR", "", repeat_count}, row_count, c_vec_column.data());
  BOOST_TEST(c_ptr_olumn.info().repeat_count() == repeat_count);
  BOOST_TEST(c_ptr_olumn.row_count() == row_count);
  BOOST_TEST(c_ptr_olumn.size() == row_count);
}

BOOST_AUTO_TEST_CASE(make_veccolumn_test)
{
  constexpr Linx::Index repeat = 3;
  constexpr Linx::Index rows = 9;
  constexpr Linx::Index size = repeat * rows;
  auto info = make_column_info<char>("NAME", "unit", repeat);
  auto vector = Test::generate_random_vector<char>(size);
  const auto* data = vector.data();
  auto column = make_column(std::move(info), std::move(vector));
  using Value = decltype(column)::Value;
  BOOST_TEST((std::is_same<Value, char>::value));
  BOOST_TEST(column.info().name == "NAME");
  BOOST_TEST(column.info().unit == "unit");
  BOOST_TEST(column.info().shape == Linx::Position<1> {repeat});
  BOOST_TEST(column.row_count() == rows);
  BOOST_TEST(column.data() == data);
}

BOOST_AUTO_TEST_CASE(make_ptrcolumn_test)
{
  constexpr Linx::Index repeat = 3;
  constexpr Linx::Index rows = 9;
  constexpr Linx::Index size = repeat * rows;
  auto info = make_column_info<char>("NAME", "unit", repeat);
  auto vector = Test::generate_random_vector<char>(size);
  auto column = make_column(std::move(info), rows, vector.data());
  using Value = decltype(column)::Value;
  BOOST_TEST((std::is_same<Value, char>::value));
  BOOST_TEST(column.info().name == "NAME");
  BOOST_TEST(column.info().unit == "unit");
  BOOST_TEST(column.info().shape == Linx::Position<1> {repeat});
  BOOST_TEST(column.row_count() == rows);
  BOOST_TEST(column.data() == vector.data());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
