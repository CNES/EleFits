// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"

namespace Euclid {
namespace Fits {

template <typename T>
std::unique_ptr<Compression> BasicCompressionStrategy::operator()(const ImageHdu::Initializer<T>& init) {

  // Too small to be compressed
  static constexpr long blockSize = 2880;
  if (shapeSize(init.shape) / sizeof(T) <= blockSize) {
    return std::make_unique<NoCompression>();
  }

  // Chain of responsibility: Plio > HCompress > Rice > ShuffledGzip
  std::unique_ptr<Compression> out;
  if (not(out = plio(init))) {
    if (not(out = hcompress(init))) {
      if (not(out = rice(init))) {
        out = gzip(init);
      }
    }
  }
  return out;
}

template <typename T>
std::unique_ptr<ShuffledGzip> BasicCompressionStrategy::gzip(const ImageHdu::Initializer<T>& init) {
  auto out = std::make_unique<ShuffledGzip>();
  out->tiling(tiling(init));
  out->quantization(quantization<T>());
  return out;
}

template <typename T>
std::unique_ptr<Rice> BasicCompressionStrategy::rice(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return nullptr;
  }
  auto out = std::make_unique<Rice>();
  out->tiling(tiling(init));
  out->quantization(quantization<T>());
  return out;
}

template <typename T>
std::unique_ptr<HCompress> BasicCompressionStrategy::hcompress(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return nullptr;
  }
  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return nullptr;
  }
  auto out = std::make_unique<HCompress>();
  out->tiling(hcompressTiling(init));
  out->quantization(quantization<T>());
  out->scaling(hcompressScaling<T>());
  return out;
}

template <typename T>
std::unique_ptr<Plio> BasicCompressionStrategy::plio(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> || m_type == Type::Lossy) {
    return nullptr;
  }
  // FIXME check values: if no data or max >= 2**24, nullptr
  auto out = std::make_unique<Plio>();
  out->tiling(tiling(init));
  return out;
}

template <typename T>
Compression::Quantization BasicCompressionStrategy::quantization() const {
  if constexpr (std::is_integral_v<T>) {
    if (m_type != Type::Lossy) {
      return Compression::Quantization(0);
    }
    Compression::Quantization q(Compression::rms / 4);
    q.dithering(Compression::Dithering::NonZeroPixel); // Keep nulls for integers
    return q;
  } else {
    if (m_type == Type::Lossless) {
      return Compression::Quantization(0);
    }
    return Compression::Quantization(Compression::rms / 4);
  }
}

template <typename T>
Position<-1> BasicCompressionStrategy::tiling(const ImageHdu::Initializer<T>& init) const {
  static constexpr long minSize = 10000;
  if (shapeSize(init.shape) <= minSize) {
    return Compression::maxTiling();
  }
  // FIXME reach minSize using higher dimensions
  return Compression::rowwiseTiling();
}

template <typename T>
Position<-1> BasicCompressionStrategy::hcompressTiling(const ImageHdu::Initializer<T>& init) const {

  const auto& shape = init.shape;

  // Small image
  if (shape[1] <= 30) {
    // FIXME what about large rows?
    return Compression::maxTiling();
  }

  // Find acceptable row count
  for (auto rows : {16, 24, 20, 30, 28, 26, 22, 18, 14}) {
    auto modulo = shape[1] % rows;
    if (modulo == 0 || modulo >= 4) {
      return Compression::rowwiseTiling(rows);
    }
  }

  // Fallback
  return Compression::rowwiseTiling(17);
}

template <typename T>
Compression::Scaling BasicCompressionStrategy::hcompressScaling() const {
  if constexpr (std::is_integral_v<T>) {
    if (m_type != Type::Lossy) {
      return 0;
    }
    return Compression::rms * 2.5;
  } else {
    if (m_type == Type::Lossless) {
      return 0;
    }
    return Compression::rms * 2.5;
  }
}

} // namespace Fits
} // namespace Euclid

#endif
