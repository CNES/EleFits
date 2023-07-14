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

template <typename TAlgo>
void testAlgoMixinCompress(long dimension, fitsfile* fptr, int comptype) {

  int status = 0;

  Euclid::Fits::Position<-1> shape(dimension);
  std::fill(shape.begin(), shape.end(), 300);

  TAlgo algo(shape);
  Cfitsio::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  BOOST_TEST(actualComptype == comptype);

  // verify tile size:
  Fits::Position<-1> actualShape(dimension);
  fits_get_tile_dim(fptr, actualShape.size(), actualShape.data(), &status);
  BOOST_TEST(actualShape == shape);

  // verify quantization level:
  float actualQlevel;
  fits_get_quantize_level(fptr, &actualQlevel, &status);
  BOOST_TEST(actualQlevel == algo.quantization().level().value());
}

template <>
void testAlgoMixinCompress<Fits::NoCompression>(long, fitsfile* fptr, int comptype) {

  int status = 0;

  Fits::NoCompression algo;
  Cfitsio::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actualComptype;
  fits_get_compression_type(fptr, &actualComptype, &status);
  BOOST_TEST(actualComptype == comptype);
}

BOOST_AUTO_TEST_CASE(algomixin_compress_test) {

  Euclid::Fits::Test::MinimalFile file;

  testAlgoMixinCompress<Fits::NoCompression>(0, file.fptr, int(NULL));

  testAlgoMixinCompress<Fits::Rice>(0, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(1, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(2, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(3, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(4, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(5, file.fptr, RICE_1);
  testAlgoMixinCompress<Fits::Rice>(6, file.fptr, RICE_1);

  //  HCompress only supports 2dim compression
  testAlgoMixinCompress<Fits::HCompress>(2, file.fptr, HCOMPRESS_1);

  testAlgoMixinCompress<Fits::Plio>(0, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(1, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(2, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(3, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(4, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(5, file.fptr, PLIO_1);
  testAlgoMixinCompress<Fits::Plio>(6, file.fptr, PLIO_1);

  testAlgoMixinCompress<Fits::Gzip>(0, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(1, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(2, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(3, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(4, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(5, file.fptr, GZIP_1);
  testAlgoMixinCompress<Fits::Gzip>(6, file.fptr, GZIP_1);

  testAlgoMixinCompress<Fits::ShuffledGzip>(0, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(1, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(2, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(3, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(4, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(5, file.fptr, GZIP_2);
  testAlgoMixinCompress<Fits::ShuffledGzip>(6, file.fptr, GZIP_2);
}

BOOST_AUTO_TEST_CASE(hcompress_compress_test) {

  int status = 0;
  Euclid::Fits::Test::MinimalFile file;

  const Euclid::Fits::Position<-1>& shape {300, 200};

  Fits::HCompress algo(shape);
  Cfitsio::compress(file.fptr, algo);

  // verify scale parameter:
  float actualScale;
  fits_get_hcomp_scale(file.fptr, &actualScale, &status);
  BOOST_TEST(
      actualScale ==
      ((algo.scaling().type() == Fits::Compression::Scaling::Type::Absolute) ?
           -algo.scaling().value() :
           algo.scaling().value()));

  // verify smoothing:
  int actualSmooth;
  fits_get_hcomp_smooth(file.fptr, &actualSmooth, &status);
  BOOST_TEST(actualSmooth == algo.isSmooth());
}

BOOST_AUTO_TEST_CASE(iscompressing_test) {

  Fits::NoCompression noneAlgo;
  const Euclid::Fits::Position<-1>& shape {300, 200};
  Fits::HCompress algo(shape);

  Euclid::Fits::Test::MinimalFile file;

  Cfitsio::compress(file.fptr, noneAlgo);
  BOOST_TEST(not Cfitsio::isCompressing(file.fptr));
  BOOST_CHECK_NO_THROW(dynamic_cast<Fits::NoCompression*>(Cfitsio::readCompression(file.fptr).get()));

  Cfitsio::compress(file.fptr, algo);
  BOOST_TEST(Cfitsio::isCompressing(file.fptr));
  const auto readAlgo = Cfitsio::readCompression(file.fptr);
  const auto readHc = dynamic_cast<Fits::HCompress&>(*readAlgo);
  BOOST_TEST(readHc.tiling() == algo.tiling()); // FIXME compare algos as a whole

  Cfitsio::compress(file.fptr, noneAlgo);
  BOOST_TEST(not Cfitsio::isCompressing(file.fptr));
  BOOST_CHECK_NO_THROW(dynamic_cast<Fits::NoCompression&>(*Cfitsio::readCompression(file.fptr)));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Compression_learning_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(default_values_learning_test) { // FIXME set compression types first

  // verify max image dimension supported for compression
  BOOST_TEST(MAX_COMPRESS_DIM == 6);

  int status = 0;
  Euclid::Fits::Test::MinimalFile file;

  int defaultAlgo;
  fits_get_compression_type(file.fptr, &defaultAlgo, &status);
  BOOST_TEST(defaultAlgo == int(NULL));

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