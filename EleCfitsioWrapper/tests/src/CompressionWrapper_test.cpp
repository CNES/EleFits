// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"

#include <boost/test/unit_test.hpp>
#include <string>

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

using namespace Euclid::Fits::Compression;

BOOST_AUTO_TEST_CASE(quantification_test) {

  Quantification quant;

  // default values:
  BOOST_TEST(quant.level() == 0.f);
  BOOST_TEST(quant.isAbsolute() == false);
  BOOST_TEST(quant.hasLossyInt() == false);
  BOOST_CHECK(quant.dither() == Dithering::EveryPixelDithering);

  // setting quantification level:
  const float positiveLevel = 5.f;
  const float zeroLevel = 0.f;
  const float negativeLevel = -5.f;

  quant.absoluteLevel(positiveLevel);
  BOOST_TEST(quant.level() == positiveLevel);
  BOOST_TEST(quant.isAbsolute() == true);

  quant.absoluteLevel(zeroLevel);
  BOOST_TEST(quant.level() == zeroLevel);
  BOOST_TEST(quant.isAbsolute() == false); // zero is always considered relative

  BOOST_CHECK_THROW(quant.absoluteLevel(negativeLevel), Euclid::Fits::FitsError);

  quant.relativeLevel(positiveLevel);
  BOOST_TEST(quant.level() == positiveLevel);
  BOOST_TEST(quant.isAbsolute() == false);

  quant.relativeLevel(zeroLevel);
  BOOST_TEST(quant.level() == zeroLevel);
  BOOST_TEST(quant.isAbsolute() == false); // zero is always considered relative

  BOOST_CHECK_THROW(quant.relativeLevel(negativeLevel), Euclid::Fits::FitsError);

  // turning on/off lossyInt:
  quant.enableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == true);
  quant.disableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == false);

  // setting dithering:
  quant.dither(Dithering::NoDithering);
  BOOST_CHECK(quant.dither() == Dithering::NoDithering);

  quant.dither(Dithering::NonZeroPixelDithering);
  BOOST_CHECK(quant.dither() == Dithering::NonZeroPixelDithering);

  quant.dither(Dithering::EveryPixelDithering);
  BOOST_CHECK(quant.dither() == Dithering::EveryPixelDithering);
}

BOOST_AUTO_TEST_CASE(scale_test) {

  const float positiveFactor = 5.f;
  const float zeroFactor = 0.f;
  const float negativeFactor = -5.f;

  const Scale absPositive_s = Scale::absolute(positiveFactor);
  const Scale absZero_s = Scale::absolute(zeroFactor);
  BOOST_CHECK_THROW(Scale::absolute(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(absPositive_s.factor() == positiveFactor);
  BOOST_TEST(absPositive_s.isAbsolute() == true);

  BOOST_TEST(absZero_s.factor() == zeroFactor);
  BOOST_TEST(absZero_s.isAbsolute() == false); // zero always considered relative

  const Scale relPositive_s = Scale::relativeToNoise(positiveFactor);
  const Scale relZero_s = Scale::relativeToNoise(zeroFactor);
  BOOST_CHECK_THROW(Scale::relativeToNoise(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(relPositive_s.factor() == positiveFactor);
  BOOST_TEST(relPositive_s.isAbsolute() == false);

  BOOST_TEST(relZero_s.factor() == zeroFactor);
  BOOST_TEST(relZero_s.isAbsolute() == false);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CompressionWrapper_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(wrapper_test) {

  // TODO
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Compression_learning_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(default_values_learning_test) {

  int status = 0;
  fitsfile* fptr;
  fits_create_file(&fptr, (std::string("!learning_test.fits")).c_str(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");

  int defaultAlgo;
  fits_get_compression_type(fptr, &defaultAlgo, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get compression type");
  BOOST_TEST(defaultAlgo == NULL);

  float defaultLevel;
  fits_get_quantize_level(fptr, &defaultLevel, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get quantize level");
  BOOST_TEST(defaultLevel == 0.0); // default should be 4.0 according to doc

  float defaultScale;
  fits_get_hcomp_scale(fptr, &defaultScale, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get hcompress scale");
  BOOST_TEST(defaultScale == 0.0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()