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

#include "EL_FitsData/Position.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Position_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(vector_init_test) {

  const std::vector<long> indices { 1, 2, 3 };

  Position<-1> braceList { indices[0], indices[1], indices[2] };
  BOOST_TEST(braceList.size() == indices.size());

  Position<-1> iterators(indices.begin(), indices.end());
  BOOST_TEST(iterators.size() == indices.size());

  Position<-1> dimension(indices.size());
  for (std::size_t i = 0; i < indices.size(); ++i) {
    dimension[i] = indices[i];
  }

  for (std::size_t i = 0; i < indices.size(); ++i) {
    BOOST_TEST(braceList[i] == indices[i]);
    BOOST_TEST(iterators[i] == indices[i]);
    BOOST_TEST(dimension[i] == indices[i]);
  }
}

BOOST_AUTO_TEST_CASE(array_init_test) {

  const std::array<long, 3> indices { 1, 2, 3 };

  Position<3> braceList { indices[0], indices[1], indices[2] };
  BOOST_TEST(braceList.size() == indices.size());

  Position<3> iterators(indices.begin(), indices.end());
  BOOST_TEST(iterators.size() == indices.size());

  Position<3> dimension(indices.size());
  for (std::size_t i = 0; i < indices.size(); ++i) {
    dimension[i] = indices[i];
  }

  for (std::size_t i = 0; i < indices.size(); ++i) {
    BOOST_TEST(braceList[i] == indices[i]);
    BOOST_TEST(iterators[i] == indices[i]);
    BOOST_TEST(dimension[i] == indices[i]);
  }
}

BOOST_AUTO_TEST_CASE(arithmetics_test) {

  const Position<4> indices { 0, 1, 2, 3 };
  const Position<4> evens { 0, 2, 4, 6 };
  const Position<4> positives { 1, 2, 3, 4 };

  const auto plus = indices + indices;
  BOOST_TEST(plus == evens);
  const auto minus = plus - indices;
  BOOST_TEST(minus == indices);

  const auto multiplies = indices * 2;
  BOOST_TEST(multiplies == evens);
  const auto divides = multiplies / 2;
  BOOST_TEST(divides == indices);

  const auto inc = indices + 1;
  BOOST_TEST(inc == positives);
  const auto dec = inc - 1;
  BOOST_TEST(dec == indices);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
