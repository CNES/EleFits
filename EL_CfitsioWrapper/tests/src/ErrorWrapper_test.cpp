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

#include "EL_CfitsioWrapper/ErrorWrapper.h"

using namespace Euclid::Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ErrorWrapper_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( noerror_test ) {

  BOOST_CHECK_NO_THROW(may_throw_cfitsio_error(0));

}

BOOST_AUTO_TEST_CASE( error_test ) {

  BOOST_CHECK_THROW(may_throw_cfitsio_error(1), CfitsioError);
  BOOST_CHECK_THROW(may_throw_cfitsio_error(1, "Unit test"), CfitsioError);

}

BOOST_AUTO_TEST_CASE( nullptr_test ) {

    BOOST_CHECK_THROW(may_throw_invalid_file_error(nullptr), CfitsioError);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


