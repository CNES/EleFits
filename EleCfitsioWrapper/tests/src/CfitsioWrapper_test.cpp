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

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/CfitsioWrapper.h"

#include <boost/test/unit_test.hpp>
#include <limits>

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
  fitsfile* fptr;
  (void)fptr;
  using FitsIO::Column;
  using FitsIO::Raster;
  using FitsIO::Record;
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
  BOOST_TEST(status == 0);
  fits_write_key(fptr, TULONG, "UNSIGNED", &unsigned_max, nullptr, &status);
  BOOST_TEST(status == 0);
  /* Read */
  unsigned long output = 0;
  fits_read_key(fptr, TULONG, "SIGNED", &output, nullptr, &status);
  BOOST_TEST(status == 0);
  BOOST_TEST(output == signed_max);
  fits_read_key(fptr, TULONG, "UNSIGNED", &output, nullptr, &status);
  BOOST_TEST(status == NUM_OVERFLOW); // CFitsIO bug
  BOOST_TEST(output != unsigned_max); // CFitsIO bug
}

BOOST_FIXTURE_TEST_CASE(resize_char_image_learning_test, Euclid::FitsIO::Test::MinimalFile) {
  // Fixture creates file and empty primary of type uchar
  int status = 0;
  long naxes = 1;
  fits_resize_img(fptr, BYTE_IMG, 1, &naxes, &status);
  BOOST_TEST(status == 0);
  fits_resize_img(fptr, SBYTE_IMG, 1, &naxes, &status);
  BOOST_TEST(status == BAD_BITPIX); // CFitsIO bug
}

BOOST_FIXTURE_TEST_CASE(resize_ulonglong_image_learning_test, Euclid::FitsIO::Test::MinimalFile) {
  // Fixture creates file and empty primary of type uchar
  int status = 0;
  long naxes = 1;
  fits_resize_img(fptr, LONGLONG_IMG, 1, &naxes, &status);
  BOOST_TEST(status == 0);
  fits_resize_img(fptr, ULONGLONG_IMG, 1, &naxes, &status);
  BOOST_TEST(status == BAD_BITPIX); // CFitsIO bug
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
