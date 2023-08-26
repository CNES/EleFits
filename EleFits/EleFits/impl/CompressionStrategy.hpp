// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"
#include "EleFitsData/DataUtils.h"

namespace Euclid {
namespace Fits {

/// @cond
template <typename TAlgo, typename T>
bool can_compress(const TAlgo&, const ImageHdu::Initializer<T>&)
{
  return std::is_base_of_v<Compression, TAlgo>;
}

template <typename T>
bool can_compress(const Rice& algo, const ImageHdu::Initializer<T>&)
{
  return std::is_integral_v<T> || not algo.is_lossless();
}

template <typename T>
bool can_compress(const HCompress& algo, const ImageHdu::Initializer<T>& init)
{
  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return false;
  }
  return std::is_integral_v<T> || not algo.is_lossless();
}

template <typename T>
bool can_compress(const Plio&, const ImageHdu::Initializer<T>& init)
{
  constexpr auto bp = bitpix<T>();

  // Float or too large int
  if constexpr (bp < 0 || bp > 32) {
    return false;
  }

  // Maybe
  if constexpr (bp > 16) {
    // Max from records
    if (init.records.has("DATAMAX")) {
      const auto max = init.records.template as<T>("DATAMAX");
      return max < (T(1) << 24);
    }

    // No max
    if (not init.data) {
      return false;
    }

    // Max from data
    const auto max = *std::max_element(init.data, init.data + shapeSize(init.shape));
    return max < (T(1) << 24);
  }

  return true;
}

template <typename TAlgo, typename T>
TAlgo& adapt_tiling(TAlgo& algo, const ImageHdu::Initializer<T>& init)
{
  static constexpr long min_size = 1024 * 1024 / sizeof(T);

  // Small image
  if (shapeSize(init.shape) <= min_size) {
    return algo.tiling(Tile::whole());
  }

  // Large image: tiles as sections
  auto tiling = unravel_index(min_size - 1, init.shape) + 1;
  for (auto i = tiling.size() - 1; i >= 0; --i) {
    if (tiling[i] > 1) {
      tiling[i] = init.shape[i];
      return algo.tiling(std::move(tiling));
    }
  }

  // FIXME throw
  return algo;
}

template <typename T>
HCompress& adapt_tiling(HCompress& algo, const ImageHdu::Initializer<T>& init)
{
  // Small image
  if (init.shape[1] <= 30) {
    // FIXME what about large rows?
    return algo.tiling(Tile::whole());
  }

  // Find acceptable row count
  for (auto rows : {16, 24, 20, 30, 28, 26, 22, 18, 14}) { // FIXME better heuristic?
    auto modulo = init.shape[1] % rows;
    if (modulo == 0 || modulo >= 4) {
      return algo.tiling(Tile::rowwise(rows));
    }
  }

  // Fallback
  return algo.tiling(Tile::rowwise(17)); // FIXME safe?
}
/// @endcond

template <typename TAlgo>
template <typename T>
bool Compress<TAlgo>::apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init)
{
  // Compress if possible
  if (auto algo = compression(init)) {
    Cfitsio::ImageCompression::compress(fptr, *algo);
    return true;
  }

  return false;
}

template <typename TAlgo>
template <typename T>
std::unique_ptr<TAlgo> Compress<TAlgo>::compression(const ImageHdu::Initializer<T>& init)
{
  // CFITSIO does not support 64-bit integer compression
  if constexpr (bitpix<T>() == 64) {
    return nullptr;
  }

  // No compression of data units less than one block long
  static constexpr std::size_t block_size = 2880;
  if (shapeSize(init.shape) * sizeof(T) <= block_size) {
    return nullptr;
  }

  // Unsuitable algorithm
  if (not can_compress(m_algo, init)) {
    return nullptr;
  }

  // Suitable algorithm
  auto out = std::make_unique<TAlgo>(m_algo);
  if (out->tiling().empty()) {
    adapt_tiling(*out, init);
  }
  return out;
}

template <typename T>
bool CompressAuto::apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init)
{
  if (auto action = plio(init)) {
    return action->apply(fptr, init);
  }

  if (auto action = hcompress(init)) {
    return action->apply(fptr, init);
  }

  if (auto action = rice(init)) {
    return action->apply(fptr, init);
  }

  return gzip(init)->apply(fptr, init);
}

template <typename T>
std::unique_ptr<Compress<ShuffledGzip>> CompressAuto::gzip(const ImageHdu::Initializer<T>&)
{
  return std::make_unique<Compress<ShuffledGzip>>(Tile::adaptive(), quantization<T>());
}

template <typename T>
std::unique_ptr<Compress<Rice>> CompressAuto::rice(const ImageHdu::Initializer<T>&)
{
  if (std::is_floating_point_v<T> && m_type == CompressionType::Lossless) {
    return nullptr;
  }
  return std::make_unique<Compress<Rice>>(Tile::adaptive(), quantization<T>());
}

template <typename T>
std::unique_ptr<Compress<HCompress>> CompressAuto::hcompress(const ImageHdu::Initializer<T>& init)
{
  if (std::is_floating_point_v<T> && m_type == CompressionType::Lossless) {
    return nullptr;
  }

  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return nullptr;
  }

  auto q = quantization<T>();
  if (q.dithering() == Quantization::Dithering::NonZeroPixel) {
    q.dithering(Quantization::Dithering::EveryPixel);
  }
  return std::make_unique<Compress<HCompress>>(Tile::adaptive(), q, hcompress_scaling<T>());
}

template <typename T>
std::unique_ptr<Compress<Plio>> CompressAuto::plio(const ImageHdu::Initializer<T>&)
{
  constexpr auto bp = bitpix<T>();

  // Float or too large int
  // Also when bp=16, it is generally not a mask, so should not use PLIO even if supported
  if constexpr (bp != 8) {
    return nullptr;
  }

  // OK
  return std::make_unique<Compress<Plio>>();
}

template <typename T>
Quantization CompressAuto::quantization() const
{
  if constexpr (std::is_integral_v<T>) {
    if (m_type != CompressionType::Lossy) {
      return Quantization(0);
    }
    Quantization q(Tile::rms / 4);
    q.dithering(Quantization::Dithering::NonZeroPixel); // Keep nulls for integers
    return q;
  } else {
    if (m_type == CompressionType::Lossless) {
      return Quantization(0);
    }
    return Quantization(Tile::rms / 16); // More conservative, imcopy default
  }
}

template <typename T>
Scaling CompressAuto::hcompress_scaling() const
{
  if constexpr (std::is_integral_v<T>) {
    if (m_type != CompressionType::Lossy) {
      return 0;
    }
    return Tile::rms * 2.5;
  } else {
    if (m_type == CompressionType::Lossless) {
      return 0;
    }
    return Tile::rms * 2.5;
  }
}

} // namespace Fits
} // namespace Euclid

#endif
