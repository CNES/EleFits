/**
 * @file tests/src/FitsDataFixture_test.cpp
 * @date 10/21/19
 * @author user
 *
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

#include "EL_FitsData//FitsDataFixture.h"

using namespace Euclid::FitsIO::Test;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FitsDataFixture_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( approx_self_test ) {
  SmallRaster a(1, 2);
  BOOST_CHECK(a.approx(a));
}

BOOST_AUTO_TEST_CASE( approx_different_shapes_test ) {
  SmallRaster a(1, 2);
  SmallRaster b(2, 3);
  BOOST_CHECK(not a.approx(b));
}

BOOST_AUTO_TEST_CASE( not_approx_test ) {
  SmallRaster a(1, 1);
  SmallRaster b(1, 1);
  b.data()[0] = 2 * (a.data()[0] + 1);
  BOOST_CHECK(not a.approx(b));
  BOOST_CHECK(not b.approx(a));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


