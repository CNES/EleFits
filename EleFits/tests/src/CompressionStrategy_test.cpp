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
  auto strategy = CompressAptly::lossless();
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
  auto strategy = CompressAptly::losslessInt();
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
  auto strategy = CompressAptly::losslessInt();
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

ELEFITS_FOREACH_RASTER_TYPE(BASIC_LOSSLESSNESS_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
