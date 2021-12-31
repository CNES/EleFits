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

#include "EleFitsData/DataContainer.h"
#include "EleFitsData/Position.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(DataContainer_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(example_test) { // FIXME test strings instead of printing
  Position<10> a {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::cout << a.slice<0>() << std::endl;
  std::cout << a.slice<1>() << std::endl;
  std::cout << a.slice<2>() << std::endl;
  std::cout << a.slice<3>() << std::endl;
  std::cout << a.slice<4>() << std::endl;
  std::cout << a.slice<5>() << std::endl;
  std::cout << a.slice<6>() << std::endl;
  std::cout << a.slice<7>() << std::endl;
  std::cout << a.slice<8>() << std::endl;
  std::cout << a.slice<9>() << std::endl;
  std::cout << a << std::endl;
  BOOST_FAIL("!!!! Please implement your tests !!!!");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
