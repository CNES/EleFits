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
  if (init.raster.data() && init.raster.size() / sizeof(T) <= blockSize) {
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
  adaptTiling(*out, init);
  adaptQuantization<T>(*out);
  return out;
}

template <typename T>
std::unique_ptr<Rice> BasicCompressionStrategy::rice(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return nullptr;
  }
  auto out = std::make_unique<Rice>();
  adaptTiling(*out, init);
  adaptQuantization<T>(*out);
  return out;
}

template <typename T>
std::unique_ptr<HCompress> BasicCompressionStrategy::hcompress(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return nullptr;
  }
  const auto shape = init.raster.shape();
  if (shape.size() < 2 || shape[0] < 4 || shape[1] < 4) {
    return nullptr;
  }
  auto out = std::make_unique<HCompress>();
  adaptHCompressTiling(*out, init);
  adaptQuantization<T>(*out);
  adaptHCompressScaling<T>(*out);
  return out;
}

template <typename T>
std::unique_ptr<Plio> BasicCompressionStrategy::plio(const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> || m_type == Type::Lossy) {
    return nullptr;
  }
  auto out = std::make_unique<Plio>();
  adaptTiling(*out, init);
  return out;
}

template <typename T>
void BasicCompressionStrategy::adaptQuantization(Compression& algo) const {
  // FIXME
}

template <typename T>
void BasicCompressionStrategy::adaptTiling(Compression& algo, const ImageHdu::Initializer<T>& init) const {
  // FIXME
}

template <typename T>
void BasicCompressionStrategy::adaptHCompressTiling(HCompress& algo, const ImageHdu::Initializer<T>& init) const {
  // FIXME
}

template <typename T>
void BasicCompressionStrategy::adaptHCompressScaling(HCompress& algo) const {
  // FIXME
}

} // namespace Fits
} // namespace Euclid

#endif
