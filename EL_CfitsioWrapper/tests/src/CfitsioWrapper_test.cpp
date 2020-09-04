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

#include <limits>

#include <boost/test/unit_test.hpp>

#include "EL_CfitsioWrapper/CfitsioFixture.h"

#include "EL_CfitsioWrapper/CfitsioWrapper.h"

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioWrapper_test)

//-----------------------------------------------------------------------------

/**
 * This is just a smoke test to ckeck that we can import the whole Cfitsio namespace
 * and get the necessary classes with a single import.
 */
BOOST_AUTO_TEST_CASE(smoke_test) {
  using namespace Euclid;
  using namespace Cfitsio;
  fitsfile *fptr;
  (void)fptr;
  using FitsIO::Record;
  using FitsIO::Column;
  using FitsIO::Raster;
}

/**
 * Check that CFitsIO is not able to read unsigned long records if greater than max(long).
 */
BOOST_FIXTURE_TEST_CASE(read_ulong_record_learning_test, Euclid::FitsIO::Test::MinimalFile) {
  /* Write */
  int status = 0;
  unsigned long signed_max = std::numeric_limits<long>::max();
  unsigned long unsigned_max = std::numeric_limits<unsigned long>::max();
  fits_write_key(fptr, TULONG, "SIGNED", &signed_max, nullptr, &status);
  BOOST_CHECK_EQUAL(status, 0);
  fits_write_key(fptr, TULONG, "UNSIGNED", &unsigned_max, nullptr, &status);
  BOOST_CHECK_EQUAL(status, 0);
  /* Read */
  unsigned long output = 0;
  fits_read_key(fptr, TULONG, "SIGNED", &output, nullptr, &status);
  BOOST_CHECK_EQUAL(status, 0);
  BOOST_CHECK_EQUAL(output, signed_max);
  fits_read_key(fptr, TULONG, "UNSIGNED", &output, nullptr, &status);
  BOOST_CHECK_NE(status, 0); // CFitsIO bug?
  BOOST_CHECK_NE(output, unsigned_max); // CFitsIO bug?
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
