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

#include <boost/test/unit_test.hpp>

#include "EL_FitsData/FitsIOError.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsIOError_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(message_test) {
  const std::string prefix = "FitsIO";
  const std::string message = "MESSAGE!";
  FitsIOError error(message);
  const std::string output = error.what();
  const auto prefixPos = output.find(prefix);
  BOOST_CHECK_NE(prefixPos, std::string::npos);
  const auto messagePos = output.find(message, prefixPos + prefix.length());
  BOOST_CHECK_NE(messagePos, std::string::npos);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
