// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
BOOST_AUTO_TEST_CASE(smoke_test)
{
  using namespace Cfitsio;
  fitsfile* fptr;
  (void)fptr;
  using Fits::Column;
  using Fits::Record;
  using Linx::Raster;
}

/**
 * Check that CFITSIO is not able to read unsigned long records if greater than max(long).
 */
BOOST_FIXTURE_TEST_CASE(read_ulong_record_learning_test, Fits::Test::MinimalFile)
{
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
  BOOST_TEST(status == NUM_OVERFLOW); // CFITSIO bug
  BOOST_TEST(output != unsigned_max); // CFITSIO bug
}

BOOST_FIXTURE_TEST_CASE(resize_char_image_learning_test, Fits::Test::MinimalFile)
{
  // Fixture creates file and empty primary of type uchar
  int status = 0;
  Linx::Index naxes = 1;
  fits_resize_img(fptr, BYTE_IMG, 1, &naxes, &status);
  BOOST_TEST(status == 0);
  fits_resize_img(fptr, SBYTE_IMG, 1, &naxes, &status);
  BOOST_TEST(status == BAD_BITPIX); // CFITSIO bug
}

BOOST_FIXTURE_TEST_CASE(resize_ulonglong_image_learning_test, Fits::Test::MinimalFile)
{
  // Fixture creates file and empty primary of type uchar
  int status = 0;
  Linx::Index naxes = 1;
  fits_resize_img(fptr, LONGLONG_IMG, 1, &naxes, &status);
  BOOST_TEST(status == 0);
  fits_resize_img(fptr, ULONGLONG_IMG, 1, &naxes, &status);
  BOOST_TEST(status == BAD_BITPIX); // CFITSIO bug
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
