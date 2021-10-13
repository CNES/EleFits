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

#include "EleFitsUtils/StringUtils.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(StringUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(split_test) {
  const std::string input = "\na\nb\nc\n\n";
  const std::vector<std::string> expected = {"", "a", "b", "c", "", ""};
  BOOST_TEST(String::split(input) == expected);
}

BOOST_AUTO_TEST_CASE(trim_test) {
  const std::string input = "\n\ta\n\tb\n c\n \n";
  const std::string expected = "a\n\tb\n c";
  BOOST_TEST(String::trim(input) == expected);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
