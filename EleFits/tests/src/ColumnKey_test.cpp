/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFits/ColumnKey.h"
#include "EleFits/TestBintable.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE(ColumnKey_test, Test::TestBintable<float>)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(from_resolved_index_test) {
  ColumnKey key(0);
  BOOST_TEST(key.index(columns) == 0);
  BOOST_TEST(key.name(columns) == scalarColumn.info().name);
}

BOOST_AUTO_TEST_CASE(from_unresolved_index_test) {
  ColumnKey key(-1);
  BOOST_TEST(key.index(columns) == 1);
  BOOST_TEST(key.name(columns) == vectorColumn.info().name);
}

BOOST_AUTO_TEST_CASE(from_name_test) {
  ColumnKey key(scalarColumn.info().name);
  BOOST_TEST(key.name(columns) == scalarColumn.info().name);
  BOOST_TEST(key.index(columns) == 0);
}

void checkIsLast(ColumnKey key, const BintableColumns& columns) {
  BOOST_TEST(key.index(columns) == 1);
}

BOOST_AUTO_TEST_CASE(implicit_cast_test) {
  checkIsLast(1, columns);
  checkIsLast(-1, columns);
  checkIsLast(lastColumn.info().name, columns);
  checkIsLast(lastColumn.info().name.c_str(), columns);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
