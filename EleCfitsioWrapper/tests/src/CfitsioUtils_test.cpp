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

#include "EleCfitsioWrapper/CfitsioUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(c_str_array_test) {
  std::vector<std::string> vec { "Fievre", "Chat", "Bebe", "Passe-miroir" };
  CStrArray arr(vec);
  for (std::size_t row = 0; row < vec.size(); ++row) {
    for (std::size_t letter = 0; letter < vec[row].length(); ++letter) {
      BOOST_TEST(vec[row].c_str()[letter] == arr.data()[row][letter]);
    }
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
