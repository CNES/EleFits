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

#include "EL_FitsData/DataUtils.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(DataUtils_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(slice_test) {
  const std::string name = "TOTOTATATITI";
  const long index = 707074747171;
  BOOST_CHECK_EQUAL(Named<int>(name).name, name);
  BOOST_CHECK_EQUAL(Indexed<int>(index).index, index);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
