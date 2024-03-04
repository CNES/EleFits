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

void check_index_to_position(long index, const Position<-1>& shape, const Position<-1>& expected)
{
  auto position = unravel_index(index, shape);
  BOOST_TEST(position == expected);
}

BOOST_AUTO_TEST_CASE(index_to_position_1d_test)
{
  for (long i = 0; i < 1024; ++i) {
    check_index_to_position(i, {1024}, {i});
  }
}

BOOST_AUTO_TEST_CASE(index_to_position_2d_test)
{
  check_index_to_position(0, {1024, 1024}, {0, 0});
  check_index_to_position(1023, {1024, 1024}, {1023, 0});
  check_index_to_position(1024, {1024, 1024}, {0, 1});
  check_index_to_position(1024 * 1024, {1024, 1024}, {0, 0}); // Cycle
}

template <typename T>
void check_adaptive_tiling(const Position<-1>& shape)
{
  Gzip algo;
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  adapt_tiling(algo, init);
  if (shape_size(shape) * sizeof(T) <= 1024 * 1024) {
    BOOST_TEST((algo.tiling() == shape || algo.tiling() == Tile::whole()));
  } else {
    BOOST_TEST(shape_size(algo.tiling()) * sizeof(T) >= 1024 * 1024);
    std::size_t dim = 6;
    for (std::size_t i = 0; i < shape.size(); ++i) {
      const auto length = algo.tiling()[i];
      if (i < dim && length != shape[i]) {
        dim = i;
      }
      if (i < dim) {
        BOOST_TEST(length == shape[i]);
      }
      if (i == dim) {
        BOOST_TEST(length <= shape[i]);
      }
      if (i > dim) {
        BOOST_TEST(length == 1);
      }
    }
  }
}

template <typename T>
void check_adaptive_tiling()
{
  check_adaptive_tiling<T>({});
  check_adaptive_tiling<T>({1});
  check_adaptive_tiling<T>({2, 2, 2, 2, 2, 2});
  check_adaptive_tiling<T>({1024 * 1024});
  check_adaptive_tiling<T>({1024 * 1024 + 1});
  check_adaptive_tiling<T>({1, 1024 * 1024});
  check_adaptive_tiling<T>({1, 1024 * 1024 + 1});
  check_adaptive_tiling<T>({1, 1024, 1024});
  check_adaptive_tiling<T>({1, 1024, 1024 + 1});
  check_adaptive_tiling<T>({1024, 1024, 1024});
  check_adaptive_tiling<T>({1024, 1024, 1024 + 1});
}

BOOST_AUTO_TEST_CASE(adaptive_tiling_test)
{
  check_adaptive_tiling<unsigned char>();
  check_adaptive_tiling<int>();
  check_adaptive_tiling<float>();
  check_adaptive_tiling<double>();
}

/**
 * @brief Whatever the type and shape, check losslessness.
 */
template <typename T>
auto check_basic_lossless(Position<-1> shape)
{
  auto strategy = CompressAuto();
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  BOOST_TEST(algo.is_lossless());
  bool none = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shape_size(shape) * sizeof(T);
  BOOST_TEST(none == (bytes <= 2880));
  if (not none && std::is_floating_point_v<T>) {
    BOOST_CHECK_NO_THROW(dynamic_cast<const ShuffledGzip&>(algo));
  }
}

/**
 * @brief Whatever the type and shape, check losslessness for integers.
 */
template <typename T>
void check_basic_lossless_ints(Position<-1> shape)
{
  auto strategy = CompressAuto(CompressionType::LosslessInts);
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  if (std::is_integral_v<T>) {
    BOOST_TEST(algo.is_lossless());
  }
  bool none = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shape_size(shape) * sizeof(T);
  BOOST_TEST(none == (bytes <= 2880));
}

template <typename T>
void check_basic_lossy(Position<-1> shape)
{
  auto strategy = CompressAuto(CompressionType::LosslessInts);
  ImageHdu::Initializer<T> init {1, "", {}, shape, nullptr};
  const auto& algo = strategy(init);
  bool none = dynamic_cast<const NoCompression*>(&algo);
  auto bytes = shape_size(shape) * sizeof(T);
  BOOST_TEST(none == (bytes <= 2880));
}

template <typename T>
void check_basic(Position<-1> shape)
{
  check_basic_lossless<T>(shape);
  check_basic_lossless_ints<T>(shape);
  check_basic_lossy<T>(shape);
}

#define BASIC_LOSSLESSNESS_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_basic_losslessness_test) \
  { \
    check_basic<type>({0}); \
    check_basic<type>({1}); \
    check_basic<type>({2879}); \
    check_basic<type>({2880}); \
    check_basic<type>({2880, 4}); \
  }

// ELEFITS_FOREACH_RASTER_TYPE(BASIC_LOSSLESSNESS_TEST) // FIXME re-enable

template <typename T, typename TAction>
void check_can_compress(TAction action)
{
  ImageHdu::Initializer<T> zero {1, "", {}, {2880 / sizeof(T)}, nullptr};
  BOOST_TEST(not action.compression(zero));
  ImageHdu::Initializer<T> one {1, "", {}, {2880 / sizeof(T), 4}, nullptr};
  BOOST_TEST(bool(action.compression(one)));
}

template <typename T, typename TAction>
void check_cannot_compress(TAction action)
{
  ImageHdu::Initializer<T> many {1, "", {}, {2880 / sizeof(T), 2880 / sizeof(T)}, nullptr};
  BOOST_TEST(not action.compression(many));
}

BOOST_AUTO_TEST_CASE(lossless_compression_ability_test)
{
  check_can_compress<std::uint16_t>(Compress<Gzip>());
  check_cannot_compress<std::int64_t>(Compress<Gzip>());
  check_can_compress<float>(Compress<Gzip>());

  check_can_compress<std::uint16_t>(Compress<ShuffledGzip>());
  check_cannot_compress<std::int64_t>(Compress<ShuffledGzip>());
  check_can_compress<float>(Compress<ShuffledGzip>());

  check_can_compress<std::uint16_t>(Compress<Rice>());
  check_cannot_compress<std::int64_t>(Compress<Rice>());
  check_cannot_compress<float>(Compress<Rice>());

  check_can_compress<std::uint16_t>(Compress<HCompress>());
  check_cannot_compress<std::int64_t>(Compress<HCompress>());
  check_cannot_compress<float>(Compress<HCompress>());

  check_can_compress<std::uint16_t>(Compress<Plio>());
  check_cannot_compress<std::int32_t>(Compress<Plio>());
  check_cannot_compress<std::int64_t>(Compress<Plio>());
  check_cannot_compress<float>(Compress<Plio>());

  check_can_compress<std::uint16_t>(CompressInts<Gzip>());
  check_cannot_compress<std::int64_t>(CompressInts<Gzip>());
  check_cannot_compress<float>(CompressInts<Gzip>());

  check_cannot_compress<std::uint16_t>(CompressFloats<Gzip>());
  check_cannot_compress<std::int64_t>(CompressFloats<Gzip>());
  check_can_compress<float>(CompressFloats<Gzip>());
}

BOOST_AUTO_TEST_CASE(lossy_compression_ability_test)
{
  check_can_compress<float>(Compress<Rice>(Tile::rowwise(), Quantization(4)));
  check_can_compress<float>(Compress<HCompress>(Tile::rowwise(16), Quantization(4)));
  check_cannot_compress<float>(Compress<Plio>(Tile::rowwise(), Quantization(4)));
}

BOOST_AUTO_TEST_CASE(plio_limit_test)
{
  using T = std::int32_t;
  constexpr T limit = T(1) << 24;
  Position<-1> shape {2881};
  Plio algo;

  ImageHdu::Initializer<T> minimal {1, "", {}, shape, nullptr};
  BOOST_TEST(not can_compress(algo, minimal));

  ImageHdu::Initializer<T> datamax_good {1, "", {{"DATAMAX", limit - 1}}, shape, nullptr};
  BOOST_TEST(can_compress(algo, datamax_good));

  ImageHdu::Initializer<T> datamax_bad {1, "", {{"DATAMAX", limit}}, shape, nullptr};
  BOOST_TEST(not can_compress(algo, datamax_bad));

  Linx::Raster<T, -1> raster_good(shape);
  raster_good[0] = limit - 1;
  ImageHdu::Initializer<T> data_good {1, "", {}, shape, raster_good.data()};
  BOOST_TEST(can_compress(algo, data_good));

  Linx::Raster<T, -1> raster_bad(shape);
  raster_bad[0] = limit;
  ImageHdu::Initializer<T> data_bad {1, "", {}, shape, raster_bad.data()};
  BOOST_TEST(not can_compress(algo, data_bad));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
