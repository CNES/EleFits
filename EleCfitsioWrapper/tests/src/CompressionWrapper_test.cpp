// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CompressionWrapper.h"

#include <boost/test/unit_test.hpp>
#include <string>

// FIXME: Elefitsdata/Compression.h and EleCfitsioWrapper/CompressionWrapper.h are both tested here
// because Compression.h cannot be imported alone

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Compression_test)

//-----------------------------------------------------------------------------

using namespace Euclid::Fits::Compression;
using namespace Euclid::Fits;

BOOST_AUTO_TEST_CASE(factor_test) {

  const float positiveFactor = 5.f;
  const float zeroFactor = 0.f;
  const float negativeFactor = -5.f;

  const Factor none_f = Factor::none();
  BOOST_TEST(none_f.value() == zeroFactor);
  BOOST_TEST(none_f.type() == Factor::Type::None);

  const Factor absolute_f = Factor::absolute(positiveFactor);
  BOOST_CHECK_THROW(Factor::absolute(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Factor::absolute(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(absolute_f.value() == positiveFactor);
  BOOST_TEST(absolute_f.type() == Factor::Type::Absolute);

  const Factor relative_f = Factor::relative(positiveFactor);
  BOOST_CHECK_THROW(Factor::relative(zeroFactor), Euclid::Fits::FitsError);
  BOOST_CHECK_THROW(Factor::relative(negativeFactor), Euclid::Fits::FitsError);

  BOOST_TEST(relative_f.value() == positiveFactor);
  BOOST_TEST(relative_f.type() == Factor::Type::Relative);

  // testing == operator:
  const Factor f1 = Factor::relative(5.f);
  const Factor f2 = Factor::relative(5.f);
  const Factor f3 = Factor::absolute(5.f);
  const Factor f4 = Factor::relative(4.f);
  BOOST_TEST((f1 == f2));
  BOOST_TEST(not(f1 == f3));
  BOOST_TEST(not(f1 == f4));
}

BOOST_AUTO_TEST_CASE(quantization_test) {

  Quantization quant;

  // default values:
  BOOST_TEST((quant.level() == Factor::none()));
  BOOST_TEST(quant.hasLossyInt() == false);
  BOOST_TEST((quant.dithering() == Dithering::EveryPixel));

  // setting quantization level:
  const float positiveLevel = 5.f;

  // FIXME: Float comparison error ?
  // quant.level(Factor::absolute(positiveLevel));
  // BOOST_TEST((quant.level().value() == Factor::absolute(positiveLevel).value()));
  // BOOST_TEST((quant.level().type() == Factor::absolute(positiveLevel).type()));
  // BOOST_TEST((quant.level() == Factor::absolute(positiveLevel)));

  quant.level(Factor::relative(positiveLevel));
  BOOST_TEST((quant.level() == Factor::relative(positiveLevel)));

  quant.level(Factor::none());
  BOOST_TEST((quant.level() == Factor::none()));

  // turning on/off lossyInt:
  quant.enableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == true);
  quant.disableLossyInt();
  BOOST_TEST(quant.hasLossyInt() == false);

  // setting dithering:
  quant.dithering(Dithering::None);
  BOOST_TEST((quant.dithering() == Dithering::None));

  quant.dithering(Dithering::NonZeroPixel);
  BOOST_TEST((quant.dithering() == Dithering::NonZeroPixel));

  quant.dithering(Dithering::EveryPixel);
  BOOST_TEST((quant.dithering() == Dithering::EveryPixel));

  // verify that chaining setters works:
  quant.level(Factor::relative(positiveLevel)).enableLossyInt().dithering(Dithering::None);
  BOOST_TEST(quant.level().value() == positiveLevel);
  BOOST_TEST(quant.level().type() == Factor::Type::Relative);
  BOOST_TEST(quant.hasLossyInt() == true);
  BOOST_TEST((quant.dithering() == Dithering::None));
}

template <typename TAlgo>
void testAlgoMixinParameters(const Position<2>& shape) {

  TAlgo algo(shape);

  // verify shape of algo is correctly stored at construction
  BOOST_TEST((algo.shape() == shape));

  // check default quantization values:
  BOOST_TEST(algo.quantize().level().value() == 0.f); // FIXME: may be changed depending on algorithm (float algos)
  BOOST_TEST(algo.quantize().level().type() == Factor::Type::None);
  BOOST_TEST(algo.quantize().hasLossyInt() == false);
  BOOST_TEST((algo.quantize().dithering() == Dithering::EveryPixel));

  // set/get quantization:
  Quantization quant;
  quant.level(Factor::absolute(5.f));
  quant.enableLossyInt();
  quant.dithering(Dithering::None);
  algo.quantize(quant);
  BOOST_TEST(algo.quantize().level().value() == quant.level().value());
  BOOST_TEST(algo.quantize().level().type() == Factor::Type::Absolute);
  BOOST_TEST(algo.quantize().hasLossyInt() == quant.hasLossyInt());
  BOOST_TEST((algo.quantize().dithering() == quant.dithering()));
}

// specific to the None algo
template <typename TAlgo>
void testAlgoMixinParameters() {

  TAlgo algo;

  // verify shape of algo is correctly stored at construction
  BOOST_TEST((algo.shape() == Position<0>()));
}

#define FOREACH_ALGO_2DIMS(MACRO, SHAPE, NDIM) \
  MACRO<None>(); \
  MACRO<Rice<NDIM>>(SHAPE); \
  MACRO<HCompress>(SHAPE); \
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
  HCompress algo(shape);

  // check default hcompress params values:
  BOOST_TEST(algo.scale().value() == 0.f);
  BOOST_TEST(algo.scale().type() == Factor::Type::None);
  BOOST_TEST(algo.isSmooth() == false);

  // setters & getters:
  Factor scale = Factor::absolute(5.f);
  algo.scale(scale);
  algo.enableSmoothing();
  BOOST_TEST(algo.scale().value() == 5.f);
  BOOST_TEST(algo.scale().type() == Factor::Type::Absolute);
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

  // verify quantization level:
  float actualQlevel;
  fits_get_quantize_level(fptr, &actualQlevel, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get quantize level");
  BOOST_TEST(actualQlevel == algo.quantize().level().value());
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
  MACRO<HCompress>(FPTR, HCOMPRESS_1, SHAPE); \
  MACRO<Plio<NDIM>>(FPTR, PLIO_1, SHAPE); \
  MACRO<Gzip<NDIM>>(FPTR, GZIP_1, SHAPE); \
  MACRO<ShuffledGzip<NDIM>>(FPTR, GZIP_2, SHAPE);

BOOST_AUTO_TEST_CASE(algomixin_compress_test) {

  int status = 0;
  fitsfile* fptr;
  fits_create_file(&fptr, (std::string("!algomixin_compress_test.fits")).c_str(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");

  const int ndim = 2;
  const Euclid::Fits::Position<ndim>& shape {300, 200};
  FOREACH_ALGO_2DIMS_COMPRESS(testAlgoMixinCompress, fptr, shape, ndim);

  // FIXME: No fitsfile close() because with it the test crashes ?
  // fits_close_file(fptr, &status);
  // Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot close file");
}

BOOST_AUTO_TEST_CASE(hcompress_compress_test) {

  int status = 0;
  fitsfile* fptr;
  fits_create_file(&fptr, (std::string("!hcompress_compress_test.fits")).c_str(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot create file");

  const Euclid::Fits::Position<2>& shape {300, 200};

  HCompress algo(shape);
  compress(fptr, algo);

  // verify scale factor:
  float actualScale;
  fits_get_hcomp_scale(fptr, &actualScale, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get hcompress scale");
  BOOST_TEST(
      actualScale == ((algo.scale().type() == Factor::Type::Absolute) ? -algo.scale().value() : algo.scale().value()));

  // verify smoothing:
  int actualSmooth;
  fits_get_hcomp_smooth(fptr, &actualSmooth, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get hcompress smooth");
  BOOST_TEST(actualSmooth == algo.isSmooth());

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