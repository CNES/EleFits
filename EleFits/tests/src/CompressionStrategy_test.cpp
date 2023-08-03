// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/CompressionStrategy.h"
#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CompressionStrategy_test)

//-----------------------------------------------------------------------------

/**
 * @brief Whatever the type and shape, check losslessness.
 */
template <typename T>
auto checkBasicLossless(Position<-1> shape) {
  auto strategy = CompressAptly();
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  BOOST_TEST(algo.isLossless());
  bool noCompression = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shapeSize(shape) * sizeof(T);
  BOOST_TEST(noCompression == (bytes <= 2880));
  if (not noCompression && std::is_floating_point_v<T>) {
    BOOST_CHECK_NO_THROW(dynamic_cast<const ShuffledGzip&>(algo));
  }
}

/**
 * @brief Whatever the type and shape, check losslessness for integers.
 */
template <typename T>
void checkBasicLosslessInt(Position<-1> shape) {
  auto strategy = CompressAptly(CompressionType::LosslessInt);
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  if (std::is_integral_v<T>) {
    BOOST_TEST(algo.isLossless());
  }
  bool noCompression = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shapeSize(shape) * sizeof(T);
  BOOST_TEST(noCompression == (bytes <= 2880));
}

template <typename T>
void checkBasicLossy(Position<-1> shape) {
  auto strategy = CompressAptly(CompressionType::LosslessInt);
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  bool noCompression = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shapeSize(shape) * sizeof(T);
  BOOST_TEST(noCompression == (bytes <= 2880));
}

template <typename T>
void checkBasic(Position<-1> shape) {
  checkBasicLossless<T>(shape);
  checkBasicLosslessInt<T>(shape);
  checkBasicLossy<T>(shape);
}

#define BASIC_LOSSLESSNESS_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_basic_losslessness_test) { \
    checkBasic<type>({0}); \
    checkBasic<type>({1}); \
    checkBasic<type>({2879}); \
    checkBasic<type>({2880}); \
    checkBasic<type>({2880, 4}); \
  }

// ELEFITS_FOREACH_RASTER_TYPE(BASIC_LOSSLESSNESS_TEST) // FIXME re-enable

template <typename T, typename TAction>
void checkCanCompress(TAction action) {
  ImageHdu::Initializer<T> zero {1, "", {}, {2880 / sizeof(T)}, nullptr};
  BOOST_TEST(not action.compression(zero));
  ImageHdu::Initializer<T> one {1, "", {}, {2880 / sizeof(T), 4}, nullptr};
  BOOST_TEST(bool(action.compression(one)));
}

template <typename T, typename TAction>
void checkCannotCompress(TAction action) {
  ImageHdu::Initializer<T> many {1, "", {}, {2880 / sizeof(T), 2880 / sizeof(T)}, nullptr};
  BOOST_TEST(not action.compression(many));
}

BOOST_AUTO_TEST_CASE(lossless_compression_ability_test) {

  checkCanCompress<std::uint16_t>(Compress<Gzip>());
  checkCannotCompress<std::int64_t>(Compress<Gzip>());
  checkCanCompress<float>(Compress<Gzip>());

  checkCanCompress<std::uint16_t>(Compress<ShuffledGzip>());
  checkCannotCompress<std::int64_t>(Compress<ShuffledGzip>());
  checkCanCompress<float>(Compress<ShuffledGzip>());

  checkCanCompress<std::uint16_t>(Compress<Rice>());
  checkCannotCompress<std::int64_t>(Compress<Rice>());
  checkCannotCompress<float>(Compress<Rice>());

  checkCanCompress<std::uint16_t>(Compress<HCompress>());
  checkCannotCompress<std::int64_t>(Compress<HCompress>());
  checkCannotCompress<float>(Compress<HCompress>());

  checkCanCompress<std::uint16_t>(Compress<Plio>());
  checkCannotCompress<std::int32_t>(Compress<Plio>());
  checkCannotCompress<std::int64_t>(Compress<Plio>());
  checkCannotCompress<float>(Compress<Plio>());

  checkCanCompress<std::uint16_t>(CompressInts<Gzip>());
  checkCannotCompress<std::int64_t>(CompressInts<Gzip>());
  checkCannotCompress<float>(CompressInts<Gzip>());

  checkCannotCompress<std::uint16_t>(CompressFloats<Gzip>());
  checkCannotCompress<std::int64_t>(CompressFloats<Gzip>());
  checkCanCompress<float>(CompressFloats<Gzip>());
}

BOOST_AUTO_TEST_CASE(lossy_compression_ability_test) {
  checkCanCompress<float>(Compress<Rice>(Tile::rowwise(), Quantization(4)));
  checkCanCompress<float>(Compress<HCompress>(Tile::rowwise(16), Quantization(4)));
  checkCannotCompress<float>(Compress<Plio>(Tile::rowwise(), Quantization(4)));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
