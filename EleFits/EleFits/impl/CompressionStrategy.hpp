// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"

namespace Euclid {
namespace Fits {

template <typename T>
const Compression& BasicCompressionStrategy::operator()(const ImageHdu::Initializer<T>& init) {

  // Too small to be compressed
  static constexpr long blockSize = 2880;
  const auto size = shapeSize(init.shape) * sizeof(T);
  if (size <= blockSize) {
    m_algo = std::make_unique<NoCompression>();
    return *m_algo;
  }
  if (size > (std::size_t(1) << 32)) {
    // FIXME enable huge_hdu
  }

  // Chain of responsibility: Plio > HCompress > Rice > ShuffledGzip
  if (not plio(init) && not hcompress(init) && not rice(init)) {
    gzip(init);
  }
  return *m_algo;
}

template <typename T>
const std::unique_ptr<Compression>& BasicCompressionStrategy::gzip(const ImageHdu::Initializer<T>& init) {
  m_algo = std::make_unique<ShuffledGzip>(tiling(init), quantization<T>());
  return m_algo;
}

template <typename T>
const std::unique_ptr<Compression>& BasicCompressionStrategy::rice(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    m_algo.reset();
    return m_algo;
  }
  m_algo = std::make_unique<Rice>(tiling(init), quantization<T>());
  return m_algo;
}

template <typename T>
const std::unique_ptr<Compression>& BasicCompressionStrategy::hcompress(const ImageHdu::Initializer<T>& init) {

  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    m_algo.reset();
    return m_algo;
  }

  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    m_algo.reset();
    return m_algo;
  }

  auto q = quantization<T>();
  if (q.dithering() == Compression::Dithering::NonZeroPixel) {
    q.dithering(Compression::Dithering::EveryPixel);
  }
  m_algo = std::make_unique<HCompress>(hcompressTiling(init), q, hcompressScaling<T>());
  return m_algo;
}

template <typename T>
const std::unique_ptr<Compression>& BasicCompressionStrategy::plio(const ImageHdu::Initializer<T>& init) {
  // if (std::is_floating_point_v<T>) {
  //   return nullptr;
  // }
  // if constexpr (bitpix<T>() >= 24) {
  //   if (not init.data) {
  //     return nullptr;
  //   }
  //   const auto max = std::max_element(init.data, init.data + shapeSize(init.shape));
  //   if (*max >= (std::size_t(1) << 24)) {
  //     return nullptr;
  //   }
  // }

  // Not sure this is a mask (very conservative)
  if constexpr (bitpix<T>() != 8) {
    m_algo.reset();
    return m_algo;
  }
  m_algo = std::make_unique<Plio>(tiling(init));
  return m_algo;
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
