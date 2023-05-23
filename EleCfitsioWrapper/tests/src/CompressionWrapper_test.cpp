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
using namespace Euclid::Fits;

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

template <typename TAlgo>
void testAlgoMixinParameters(const Position<2>& shape) {

  TAlgo algo(shape);

  // verify shape of algo is correctly stored at construction
  BOOST_CHECK(algo.shape() == shape);

  // check default quantification values:
  BOOST_TEST(algo.quantize().level() == 0.f); // FIXME: may be changed depending on algorithm (float algos)
  BOOST_TEST(algo.quantize().isAbsolute() == false);
  BOOST_TEST(algo.quantize().hasLossyInt() == false);
  BOOST_CHECK(algo.quantize().dither() == Dithering::EveryPixelDithering);

  // set/get quantification:
  Quantification quant;
  quant.absoluteLevel(5.f);
  quant.enableLossyInt();
  quant.dither(Dithering::NoDithering);
  algo.quantize(quant);
  BOOST_TEST(algo.quantize().level() == quant.level());
  BOOST_TEST(algo.quantize().isAbsolute() == quant.isAbsolute());
  BOOST_TEST(algo.quantize().hasLossyInt() == quant.hasLossyInt());
  BOOST_CHECK(algo.quantize().dither() == quant.dither());
}

// specific to the None algo
template <typename TAlgo>
void testAlgoMixinParameters() {

  TAlgo algo;

  // verify shape of algo is correctly stored at construction
  BOOST_CHECK(algo.shape() == Position<0>());
}

#define FOREACH_ALGO_2DIMS(MACRO, SHAPE, NDIM) \
  MACRO<None>(); \
  MACRO<Rice<NDIM>>(SHAPE); \
  MACRO<HCompress<NDIM>>(SHAPE); \
  MACRO<Plio<NDIM>>(SHAPE); \
  MACRO<Gzip<NDIM>>(SHAPE); \
  MACRO<ShuffledGzip<NDIM>>(SHAPE);

BOOST_AUTO_TEST_CASE(algo_mixin_test) {

  const int ndim = 2;
  const Position<ndim>& shape {300, 200};
  FOREACH_ALGO_2DIMS(testAlgoMixinParameters, shape, ndim);
}

BOOST_AUTO_TEST_CASE(hcompress_test) {

  const Position<2>& shape {300, 200};
  HCompress<2> algo(shape);

  // check default hcompress params values:
  BOOST_TEST(algo.scale().factor() == 0.f);
  BOOST_TEST(algo.scale().isAbsolute() == false);
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Scale scale = Scale::absolute(5.f);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST(algo.scale().factor() == 5.f);
  BOOST_TEST(algo.scale().isAbsolute() == true);
  BOOST_TEST(algo.isSmooth() == true);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CompressionWrapper_test)

//-----------------------------------------------------------------------------

using namespace Euclid::Fits::Compression;
using namespace Euclid::Cfitsio::Compression;

template <typename TAlgo>
void testAlgoMixinCompress(fitsfile* fptr, int comptype, const Euclid::Fits::Position<2>& shape) {

  int status = 0;

  TAlgo algo(shape);
  compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get compression type");
  BOOST_TEST(actualComptype == comptype);

  // verify tile size:
  long actualShape[2];
  fits_get_tile_dim(fptr, 2, actualShape, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get tile dim");
  BOOST_TEST(actualShape[0] = shape.data()[0]);
  BOOST_TEST(actualShape[1] = shape.data()[1]);

  // verify default quantification level:
  float qlevel;
  fits_get_quantize_level(fptr, &qlevel, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get quantize level");
  BOOST_TEST(qlevel == algo.quantize().level());
}

// specific to the None algo
template <typename TAlgo>
void testAlgoMixinCompress(fitsfile* fptr, int comptype) {

  int status = 0;

  TAlgo algo;
  compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get compression type");
  BOOST_TEST(actualComptype == comptype);
}

#define FOREACH_ALGO_2DIMS_COMPRESS(MACRO, FPTR, SHAPE, NDIM) \
  MACRO<None>(FPTR, NULL); \
  MACRO<Rice<NDIM>>(FPTR, RICE_1, SHAPE); \
  MACRO<HCompress<NDIM>>(FPTR, HCOMPRESS_1, SHAPE); \
  MACRO<Plio<NDIM>>(FPTR, PLIO_1, SHAPE); \
  MACRO<Gzip<NDIM>>(FPTR, GZIP_1, SHAPE); \
  MACRO<ShuffledGzip<NDIM>>(FPTR, GZIP_2, SHAPE);

BOOST_AUTO_TEST_CASE(wrapper_test) {

  int status = 0;
  fitsfile* fptr;
  fits_create_file(&fptr, (std::string("!wrapper_test.fits")).c_str(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");

  const int ndim = 2;
  const Euclid::Fits::Position<ndim>& shape {300, 200};
  FOREACH_ALGO_2DIMS_COMPRESS(testAlgoMixinCompress, fptr, shape, ndim);

  // FIXME: No fitsfile close() because with it the test crashes ?
  // fits_close_file(fptr, &status);
  // Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Compression_learning_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(default_values_learning_test) {

  // verify max image dimension supported for compression
  BOOST_TEST(MAX_COMPRESS_DIM == 6);

  int status = 0;
  fitsfile* fptr;
  fits_create_file(&fptr, (std::string("!learning_test.fits")).c_str(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");

  int defaultAlgo;
  fits_get_compression_type(fptr, &defaultAlgo, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get compression type");
  BOOST_TEST(defaultAlgo == NULL);

  long defaultTileDim[MAX_COMPRESS_DIM];
  fits_get_tile_dim(fptr, MAX_COMPRESS_DIM, defaultTileDim, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get tile dim");
  for (int i = 0; i < MAX_COMPRESS_DIM; i++) {
    BOOST_TEST(defaultTileDim[i] == 0); // ie. defaultTileDim = {0,0,0,0,0,0}
  }

  float defaultLevel;
  fits_get_quantize_level(fptr, &defaultLevel, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get quantize level");
  BOOST_TEST(defaultLevel == 0.0);

  float defaultScale;
  fits_get_hcomp_scale(fptr, &defaultScale, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get hcompress scale");
  BOOST_TEST(defaultScale == 0.0);

  // FIXME: No fitsfile close() because with it the test crashes ?
  // fits_close_file(fptr, &status);
  // Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()