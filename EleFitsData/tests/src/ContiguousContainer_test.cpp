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

#include "EleFitsData/ContiguousContainer.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ContiguousContainer_test)

struct TestContiguousContainer : ContiguousContainerMixin<int, TestContiguousContainer> {

  TestContiguousContainer(std::size_t size = 0, const int* data = nullptr) : vector() {
    if (size > 0) {
      if (data) {
        vector.assign(data, data + size);
      } else {
        vector.resize(size);
      }
    }
  }

  const int* data() const {
    return vector.data();
  }

  std::size_t size() const {
    return vector.size();
  }

  std::vector<int> vector;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(mixin_test) {

  constexpr std::size_t size = 10;
  TestContiguousContainer tcc(size);
  TestContiguousContainer empty;

  BOOST_TEST(not tcc.emtpy());
  BOOST_TEST(&tcc[size / 2] == &tcc.vector[size / 2]);
  BOOST_TEST(tcc.begin() == tcc.vector.data());
  BOOST_TEST(tcc.cbegin() == tcc.vector.data());
  BOOST_TEST(tcc.end() == tcc.vector.data() + size);
  BOOST_TEST(tcc.cend() == tcc.vector.data() + size);
  BOOST_TEST(tcc == tcc);
  BOOST_TEST(empty == empty);
  BOOST_TEST(tcc != empty);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
