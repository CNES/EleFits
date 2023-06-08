// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/CompressionWrapper.h"

#include <boost/test/unit_test.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(CompressionWrapper_test)

//-----------------------------------------------------------------------------

using namespace Euclid;

template <typename TAlgo, long NDim>
void testAlgoMixinCompress(fitsfile* fptr, int comptype) {

  int status = 0;

  Euclid::Fits::Position<NDim> shape;
  for (int i = 0; i < NDim; i++) {
    shape[i] = 300;
  }

  TAlgo algo(shape);
  Cfitsio::Compression::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  BOOST_TEST(actualComptype == comptype);

  // verify tile size:
  long actualShape[NDim];
  fits_get_tile_dim(fptr, NDim, actualShape, &status);
  for (int i = 0; i < NDim; i++) {
    BOOST_TEST(actualShape[i] = shape.data()[i]);
  }

  // verify quantization level:
  float actualQlevel;
  fits_get_quantize_level(fptr, &actualQlevel, &status);
  BOOST_TEST(actualQlevel == algo.quantize().level().value());
}

// specific to the None algo
template <typename TAlgo>
void testAlgoMixinCompress(fitsfile* fptr, int comptype) {

  int status = 0;

  TAlgo algo;
  Cfitsio::Compression::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  BOOST_TEST(actualComptype == comptype);
}

BOOST_AUTO_TEST_CASE(algomixin_compress_test) {

  Euclid::Fits::Test::MinimalFile file;

  testAlgoMixinCompress<Fits::Compression::None>(file.fptr, NULL);

  testAlgoMixinCompress<Fits::Compression::Rice<0>, 0>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<1>, 1>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<2>, 2>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<3>, 3>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<4>, 4>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<5>, 5>(file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Compression::Rice<6>, 6>(file.fptr, RICE_1);

  //  HCompress only supports 2dim compression
  testAlgoMixinCompress<Fits::Compression::HCompress, 2>(file.fptr, HCOMPRESS_1);

  testAlgoMixinCompress<Fits::Compression::Plio<0>, 0>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<1>, 1>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<2>, 2>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<3>, 3>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<4>, 4>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<5>, 5>(file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Compression::Plio<6>, 6>(file.fptr, PLIO_1);

  testAlgoMixinCompress<Fits::Compression::Gzip<0>, 0>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<1>, 1>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<2>, 2>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<3>, 3>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<4>, 4>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<5>, 5>(file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Compression::Gzip<6>, 6>(file.fptr, GZIP_1);

  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<0>, 0>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<1>, 1>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<2>, 2>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<3>, 3>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<4>, 4>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<5>, 5>(file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::Compression::ShuffledGzip<6>, 6>(file.fptr, GZIP_2);
}

BOOST_AUTO_TEST_CASE(hcompress_compress_test) {

  int status = 0;
  Euclid::Fits::Test::MinimalFile file;

  const Euclid::Fits::Position<2>& shape {300, 200};

  Fits::Compression::HCompress algo(shape);
  Cfitsio::Compression::compress(file.fptr, algo);

  // verify scale factor:
  float actualScale;
  fits_get_hcomp_scale(file.fptr, &actualScale, &status);
  BOOST_TEST(
      actualScale ==
      ((algo.scale().type() == Fits::Compression::Factor::Type::Absolute) ?
           -algo.scale().value() :
           algo.scale().value()));

  // verify smoothing:
  int actualSmooth;
  fits_get_hcomp_smooth(file.fptr, &actualSmooth, &status);
  BOOST_TEST(actualSmooth == algo.isSmooth());
}

BOOST_AUTO_TEST_CASE(iscompressing_test) {

  Fits::Compression::None noneAlgo;
  const Euclid::Fits::Position<2>& shape {300, 200};
  Fits::Compression::HCompress algo(shape);

  Euclid::Fits::Test::MinimalFile file;

  Cfitsio::Compression::compress(file.fptr, noneAlgo);
  BOOST_TEST(Cfitsio::Compression::isCompressing(file.fptr) == false);

  Cfitsio::Compression::compress(file.fptr, algo);
  BOOST_TEST(Cfitsio::Compression::isCompressing(file.fptr) == true);

  Cfitsio::Compression::compress(file.fptr, noneAlgo);
  BOOST_TEST(Cfitsio::Compression::isCompressing(file.fptr) == false);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Compression_learning_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(default_values_learning_test) {

  // verify max image dimension supported for compression
  BOOST_TEST(MAX_COMPRESS_DIM == 6);

  int status = 0;
  Euclid::Fits::Test::MinimalFile file;

  int defaultAlgo;
  fits_get_compression_type(file.fptr, &defaultAlgo, &status);
  BOOST_TEST(defaultAlgo == NULL);

  long defaultTileDim[MAX_COMPRESS_DIM];
  fits_get_tile_dim(file.fptr, MAX_COMPRESS_DIM, defaultTileDim, &status);
  for (int i = 0; i < MAX_COMPRESS_DIM; i++) {
    BOOST_TEST(defaultTileDim[i] == 0); // ie. defaultTileDim = {0,0,0,0,0,0}
  }

  float defaultLevel;
  fits_get_quantize_level(file.fptr, &defaultLevel, &status);
  BOOST_TEST(defaultLevel == 0.0);

  float defaultScale;
  fits_get_hcomp_scale(file.fptr, &defaultScale, &status);
  BOOST_TEST(defaultScale == 0.0);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()