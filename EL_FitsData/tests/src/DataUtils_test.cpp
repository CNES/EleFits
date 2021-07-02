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

#include "EL_FitsData/DataUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

template <typename T>
struct PassBySpy {

  PassBySpy(T v) : value(v), moved(false), copied(false) {}

  PassBySpy(const PassBySpy& p) : value(p.value), moved(false), copied(true) {}

  PassBySpy(PassBySpy&& p) : value(p.value), moved(true), copied(false) {}

  ~PassBySpy() = default;

  PassBySpy& operator=(const PassBySpy& p) {
    value = p.value;
    copied = true;
  }

  PassBySpy& operator=(PassBySpy&& p) {
    value = p.value;
    moved = true;
  }

  T value;
  bool moved;
  bool copied;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(DataUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(slice_test) {
  const std::string name = "TOTOTATATITI";
  const long index = 707074747171;
  BOOST_TEST(Named<int>(name).name == name);
  BOOST_TEST(Indexed<int>(index).index == index);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
