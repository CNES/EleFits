// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"
#include "EleFitsData/DataUtils.h"

namespace Euclid {
namespace Fits {

template <typename T>
bool CompressAptly::apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {

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
std::unique_ptr<Compress<ShuffledGzip>> CompressAptly::gzip(const ImageHdu::Initializer<T>& init) {
  return std::make_unique<Compress<ShuffledGzip>>(tiling(init), quantization<T>());
}

template <typename T>
std::unique_ptr<Compress<Rice>> CompressAptly::rice(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == CompressionType::Lossless) {
    return nullptr;
  }
  return std::make_unique<Compress<Rice>>(tiling(init), quantization<T>());
}

template <typename T>
std::unique_ptr<Compress<HCompress>> CompressAptly::hcompress(const ImageHdu::Initializer<T>& init) {

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
  return std::make_unique<Compress<HCompress>>(hcompress_tiling(init), q, hcompress_scaling<T>());
}

template <typename T>
std::unique_ptr<Compress<Plio>> CompressAptly::plio(const ImageHdu::Initializer<T>& init) {

  constexpr auto bp = bitpix<T>();

  // Float or too large int
  if constexpr (bp < 0 || bp > 32) {
    return nullptr;
  }

  // Maybe
  if constexpr (bp > 16) {
    if (not init.data) {
      return nullptr;
    }
    const auto max = *std::max_element(init.data, init.data + shapeSize(init.shape));
    if (max >= (T(1) << 24)) {
      return nullptr;
    }
  }

  // OK
  return std::make_unique<Compress<Plio>>(tiling(init));
}

template <typename T>
Quantization CompressAptly::quantization() const {
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
Position<-1> CompressAptly::tiling(const ImageHdu::Initializer<T>& init) const {

  static constexpr long min_size = 1024 * 1024;
  if (shapeSize(init.shape) <= min_size) {
    return Tile::whole();
  }

  const long row_width = init.shape[0];
  const long row_size = row_width * sizeof(T);
  const long row_count = min_size / row_size + 1;

  // FIXME reach min_size using higher dimensions

  return {row_width, row_count};
}

template <typename T>
Position<-1> CompressAptly::hcompress_tiling(const ImageHdu::Initializer<T>& init) const {

  const auto& shape = init.shape;

  // Small image
  if (shape[1] <= 30) {
    // FIXME what about large rows?
    return Tile::whole();
  }

  // Find acceptable row count
  for (auto rows : {16, 24, 20, 30, 28, 26, 22, 18, 14}) {
    auto modulo = shape[1] % rows;
    if (modulo == 0 || modulo >= 4) {
      return Tile::rowwise(rows);
    }
  }

  // Fallback
  return Tile::rowwise(17);
}

template <typename T>
Scaling CompressAptly::hcompress_scaling() const {
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
