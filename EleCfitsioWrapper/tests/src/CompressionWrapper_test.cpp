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
void test_algo_mixin_compress(long dimension, fitsfile* fptr, int comptype) {

  int status = 0;

  Euclid::Fits::Position<-1> shape(dimension);
  std::fill(shape.begin(), shape.end(), 300);

  TAlgo algo(shape);
  Cfitsio::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actual_comptype;
  fits_get_compression_type(fptr, &actual_comptype, &status);
  BOOST_TEST(actual_comptype == comptype);

  // verify tile size:
  Fits::Position<-1> actual_shape(dimension);
  fits_get_tile_dim(fptr, actual_shape.size(), actual_shape.data(), &status);
  BOOST_TEST(actual_shape == shape);

  // verify quantization level:
  float actual_qlevel;
  fits_get_quantize_level(fptr, &actual_qlevel, &status);
  BOOST_TEST(actual_qlevel == algo.quantization().level().value());
}

template <>
void test_algo_mixin_compress<Fits::NoCompression>(long, fitsfile* fptr, int comptype) {

  int status = 0;

  Fits::NoCompression algo;
  Cfitsio::compress(fptr, algo);

  // verify the correct compression algo is set:
  int actual_comptype;
  fits_get_compression_type(fptr, &actual_comptype, &status);
  BOOST_TEST(actual_comptype == comptype);
}

BOOST_AUTO_TEST_CASE(algomixin_compress_test) {

  Euclid::Fits::Test::MinimalFile file;

  test_algo_mixin_compress<Fits::NoCompression>(0, file.fptr, int(NULL));

  test_algo_mixin_compress<Fits::Rice>(0, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(1, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(2, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(3, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(4, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(5, file.fptr, RICE_1);
  test_algo_mixin_compress<Fits::Rice>(6, file.fptr, RICE_1);

  //  HCompress only supports 2dim compression
  test_algo_mixin_compress<Fits::HCompress>(2, file.fptr, HCOMPRESS_1);

  test_algo_mixin_compress<Fits::Plio>(0, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(1, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(2, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(3, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(4, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(5, file.fptr, PLIO_1);
  test_algo_mixin_compress<Fits::Plio>(6, file.fptr, PLIO_1);

  test_algo_mixin_compress<Fits::Gzip>(0, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(1, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(2, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(3, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(4, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(5, file.fptr, GZIP_1);
  test_algo_mixin_compress<Fits::Gzip>(6, file.fptr, GZIP_1);

  test_algo_mixin_compress<Fits::ShuffledGzip>(0, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(1, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(2, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(3, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(4, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(5, file.fptr, GZIP_2);
  test_algo_mixin_compress<Fits::ShuffledGzip>(6, file.fptr, GZIP_2);
}

BOOST_AUTO_TEST_CASE(hcompress_compress_test) {

  int status = 0;
  Euclid::Fits::Test::MinimalFile file;

  const Euclid::Fits::Position<-1>& shape {300, 200};

  Fits::HCompress algo(shape);
  Cfitsio::compress(file.fptr, algo);

  // verify scale parameter:
  float actual_scale;
  fits_get_hcomp_scale(file.fptr, &actual_scale, &status);
  BOOST_TEST(
      actual_scale ==
      ((algo.scaling().type() == Fits::Scaling::Type::Absolute) ? -algo.scaling().value() : algo.scaling().value()));

  // verify smoothing:
  int actual_smoothing;
  fits_get_hcomp_smooth(file.fptr, &actual_smoothing, &status);
  BOOST_TEST(actual_smoothing == algo.is_smooth());
}

BOOST_AUTO_TEST_CASE(iscompressing_test) {

  Fits::NoCompression none;
  const Euclid::Fits::Position<-1>& shape {300, 200};
  Fits::HCompress algo(shape);

  Euclid::Fits::Test::MinimalFile file;

  Cfitsio::compress(file.fptr, none);
  BOOST_TEST(not Cfitsio::is_compressing(file.fptr));
  BOOST_CHECK_NO_THROW(dynamic_cast<Fits::NoCompression*>(Cfitsio::get_compression(file.fptr).get()));

  Cfitsio::compress(file.fptr, algo);
  BOOST_TEST(Cfitsio::is_compressing(file.fptr));
  const auto read_algo = Cfitsio::get_compression(file.fptr);
  const auto read_hc = dynamic_cast<Fits::HCompress&>(*read_algo);
  BOOST_TEST(read_hc.tiling() == algo.tiling()); // FIXME compare algos as a whole

  Cfitsio::compress(file.fptr, none);
  BOOST_TEST(not Cfitsio::is_compressing(file.fptr));
  BOOST_CHECK_NO_THROW(dynamic_cast<Fits::NoCompression&>(*Cfitsio::get_compression(file.fptr)));
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

  int default_algo;
  fits_get_compression_type(file.fptr, &default_algo, &status);
  BOOST_TEST(default_algo == int(NULL));

  long default_tile_dim[MAX_COMPRESS_DIM];
  fits_get_tile_dim(file.fptr, MAX_COMPRESS_DIM, default_tile_dim, &status);
  for (int i = 0; i < MAX_COMPRESS_DIM; i++) {
    BOOST_TEST(default_tile_dim[i] == 0); // ie. default_tile_dim = {0,0,0,0,0,0}
  }

  float default_qlevel;
  fits_get_quantize_level(file.fptr, &default_qlevel, &status);
  BOOST_TEST(default_qlevel == 0.0);

  float default_scale;
  fits_get_hcomp_scale(file.fptr, &default_scale, &status);
  BOOST_TEST(default_scale == 0.0);
}

BOOST_AUTO_TEST_CASE(default_tiling_following_hcompress_test) {
  Euclid::Fits::Test::MinimalFile file;
  int status = 0;
  int tile1, tile2;
  long shape[2] = {100, 100};
  long tiling[2] = {-1, 1};
  fits_set_compression_type(file.fptr, HCOMPRESS_1, &status);
  fits_set_tile_dim(file.fptr, 2, tiling, &status);
  fits_create_img(file.fptr, BYTE_IMG, 2, shape, &status);
  fits_read_key(file.fptr, TINT, "ZTILE1", &tile1, nullptr, &status);
  fits_read_key(file.fptr, TINT, "ZTILE2", &tile2, nullptr, &status);
  BOOST_TEST(tile1 == shape[0]);
  BOOST_TEST(tile2 == 16);

  fits_set_compression_type(file.fptr, GZIP_2, &status);
  fits_set_tile_dim(file.fptr, 2, tiling, &status);
  fits_create_img(file.fptr, BYTE_IMG, 2, shape, &status);
  fits_read_key(file.fptr, TINT, "ZTILE1", &tile1, nullptr, &status);
  fits_read_key(file.fptr, TINT, "ZTILE2", &tile2, nullptr, &status);
  BOOST_TEST(tile1 == shape[0]);
  BOOST_TEST(tile2 == 1);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()