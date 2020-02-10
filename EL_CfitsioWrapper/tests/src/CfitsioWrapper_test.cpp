/**
 * @file tests/src/CfitsioWrapper_test.cpp
 * @date 08/06/19
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

#include "EL_CfitsioWrapper/CfitsioWrapper.h"

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CfitsioWrapper_test)

//-----------------------------------------------------------------------------

/**
 * This is just a smoke test to ckeck that we can import the whole Cfitsio namespace
 * and get the necessary classes with a single import.
 */
BOOST_AUTO_TEST_CASE( smoke_test ) {
  using namespace Euclid;
  using namespace Cfitsio;
  fitsfile* fptr;
  (void)fptr;
  using FitsIO::Record;
  using FitsIO::Column;
  using FitsIO::Raster;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
