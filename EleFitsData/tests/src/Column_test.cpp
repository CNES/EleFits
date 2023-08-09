// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Column.h"
#include "EleFitsData/TestColumn.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Column_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(column_data_can_be_shared_test) {
  std::vector<int> input {1, 2, 3};
  PtrColumn<int> column({"SHARED", "", 1}, input.size(), input.data());
  BOOST_TEST(column.data()[1] == 2);
  input[1] = 4;
  BOOST_TEST(column.data()[1] == 4);
}

BOOST_AUTO_TEST_CASE(column_data_can_be_moved_test) {
  std::vector<int> input {4, 5, 6};
  VecColumn<int> column({"DATA", "", 1}, std::move(input));
  BOOST_TEST(column.vector()[1] == 5);
  BOOST_TEST(input.size() == 0);
  column.moveTo(input);
  BOOST_TEST(input[1] == 5);
  BOOST_TEST(column.vector().size() == 0);
  BOOST_TEST(column.elementCount() == 0);
}

BOOST_AUTO_TEST_CASE(subscript_bounds_test) {
  const long row_count = 10;
  const long repeatCount = 3;
  Test::RandomVectorColumn<int> column(repeatCount, row_count);
  column.at(1, -1) = 1;
  BOOST_TEST(column.at(1, -1) == 1);
  const auto& vec = column.vector();
  BOOST_TEST(column.at(0) == vec[0]);
  BOOST_TEST(column.at(-1) == vec[(row_count - 1) * repeatCount]);
  BOOST_TEST(column.at(-row_count) == vec[0]);
  BOOST_TEST(column.at(0, -1) == vec[repeatCount - 1]);
  BOOST_TEST(column.at(-1, -1) == vec[row_count * repeatCount - 1]);
  BOOST_CHECK_THROW(column.at(row_count), FitsError);
  BOOST_CHECK_THROW(column.at(-1 - row_count), FitsError);
  BOOST_CHECK_THROW(column.at(0, repeatCount), FitsError);
  BOOST_CHECK_THROW(column.at(0, -1 - repeatCount), FitsError);
}

BOOST_AUTO_TEST_CASE(numeric_column_elementcount_is_rowcount_times_repeatcount_test) {

  constexpr long row_count = 17;
  constexpr long repeatCount = 7;
  // FIXME Breaks with int instead of long:
  // CTor overload forwards to vector and creates 17 values instead of 17 * 7

  /* VecColumn */
  VecColumn<float> vecColumn({"STR", "", repeatCount}, row_count);
  BOOST_TEST(vecColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(vecColumn.rowCount() == row_count);
  BOOST_TEST(vecColumn.elementCount() == row_count * repeatCount);

  /* PtrColumn */
  PtrColumn<float> ptrColumn({"STR", "", repeatCount}, row_count, vecColumn.data());
  BOOST_TEST(ptrColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(ptrColumn.rowCount() == row_count);
  BOOST_TEST(ptrColumn.elementCount() == row_count * repeatCount);

  /* Constant VecColumn */
  const VecColumn<float> cVecColumn({"STR", "", repeatCount}, row_count);
  BOOST_TEST(cVecColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(cVecColumn.rowCount() == row_count);
  BOOST_TEST(cVecColumn.elementCount() == row_count * repeatCount);

  /* Constant PtrColumn */
  const PtrColumn<const float> cPtrColumn({"STR", "", repeatCount}, row_count, cVecColumn.data());
  BOOST_TEST(cPtrColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(cPtrColumn.rowCount() == row_count);
  BOOST_TEST(cPtrColumn.elementCount() == row_count * repeatCount);
}

BOOST_AUTO_TEST_CASE(string_column_elementcount_is_rowcount_test) {

  constexpr long row_count = 17;
  constexpr long repeatCount = 7;

  /* VecColumn */
  VecColumn<std::string> vecColumn({"STR", "", repeatCount}, row_count);
  BOOST_TEST(vecColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(vecColumn.rowCount() == row_count);
  BOOST_TEST(vecColumn.elementCount() == row_count);

  /* PtrColumn */
  PtrColumn<std::string> ptrColumn({"STR", "", repeatCount}, row_count, vecColumn.data());
  BOOST_TEST(ptrColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(ptrColumn.rowCount() == row_count);
  BOOST_TEST(ptrColumn.elementCount() == row_count);

  /* Constant VecColumn */
  const VecColumn<std::string> cVecColumn({"STR", "", repeatCount}, row_count);
  BOOST_TEST(cVecColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(cVecColumn.rowCount() == row_count);
  BOOST_TEST(cVecColumn.elementCount() == row_count);

  /* Constant PtrColumn */
  const PtrColumn<const std::string> cPtrColumn({"STR", "", repeatCount}, row_count, cVecColumn.data());
  BOOST_TEST(cPtrColumn.info().repeatCount() == repeatCount);
  BOOST_TEST(cPtrColumn.rowCount() == row_count);
  BOOST_TEST(cPtrColumn.elementCount() == row_count);
}

BOOST_AUTO_TEST_CASE(make_veccolumn_test) {
  constexpr long repeat = 3;
  constexpr long rows = 9;
  constexpr long size = repeat * rows;
  auto info = makeColumnInfo<char>("NAME", "unit", repeat);
  auto vector = Test::generate_random_vector<char>(size);
  const auto* data = vector.data();
  auto column = makeColumn(std::move(info), std::move(vector));
  using Value = decltype(column)::Value;
  BOOST_TEST((std::is_same<Value, char>::value));
  BOOST_TEST(column.info().name == "NAME");
  BOOST_TEST(column.info().unit == "unit");
  BOOST_TEST(column.info().shape == Position<1> {repeat});
  BOOST_TEST(column.rowCount() == rows);
  BOOST_TEST(column.data() == data);
}

BOOST_AUTO_TEST_CASE(make_ptrcolumn_test) {
  constexpr long repeat = 3;
  constexpr long rows = 9;
  constexpr long size = repeat * rows;
  auto info = makeColumnInfo<char>("NAME", "unit", repeat);
  auto vector = Test::generate_random_vector<char>(size);
  auto column = makeColumn(std::move(info), rows, vector.data());
  using Value = decltype(column)::Value;
  BOOST_TEST((std::is_same<Value, char>::value));
  BOOST_TEST(column.info().name == "NAME");
  BOOST_TEST(column.info().unit == "unit");
  BOOST_TEST(column.info().shape == Position<1> {repeat});
  BOOST_TEST(column.rowCount() == rows);
  BOOST_TEST(column.data() == vector.data());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
