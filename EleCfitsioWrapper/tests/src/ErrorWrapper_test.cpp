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

#include "EleCfitsioWrapper/ErrorWrapper.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ErrorWrapper_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(noerror_test) {
  BOOST_CHECK_NO_THROW(CfitsioError::mayThrow(0));
}

BOOST_AUTO_TEST_CASE(error_test) {
  BOOST_CHECK_THROW(CfitsioError::mayThrow(1), CfitsioError);
}

BOOST_AUTO_TEST_CASE(nullptr_test) {
  BOOST_CHECK_THROW(mayThrowInvalidFileError(nullptr), CfitsioError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
