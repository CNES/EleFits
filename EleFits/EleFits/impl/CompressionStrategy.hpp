// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"

namespace Euclid {
namespace Fits {

template <typename T>
bool CompressAptly::operator()(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {

  // Too small to be compressed
  static constexpr long blockSize = 2880;
  const auto size = shapeSize(init.shape) * sizeof(T);
  if (size <= blockSize) {
    return false;
  }
  if (size > (std::size_t(1) << 32)) {
    // FIXME enable huge_hdu
  }

  // Chain of responsibility: Plio > HCompress > Rice > ShuffledGzip
  if (not plio(fptr, init) && not hcompress(fptr, init) && not rice(fptr, init)) {
    gzip(fptr, init);
  }
  return true;
}

template <typename T>
bool CompressAptly::gzip(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {
  Compress<ShuffledGzip>(tiling(init), quantization<T>())(fptr, init);
  return true;
}

template <typename T>
bool CompressAptly::rice(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {
  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return false;
  }
  Compress<Rice>(tiling(init), quantization<T>())(fptr, init);
  return true;
}

template <typename T>
bool CompressAptly::hcompress(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {

  if (std::is_floating_point_v<T> && m_type == Type::Lossless) {
    return false;
  }

  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return false;
  }

  auto q = quantization<T>();
  if (q.dithering() == Quantization::Dithering::NonZeroPixel) {
    q.dithering(Quantization::Dithering::EveryPixel);
  }
  Compress<HCompress>(hcompressTiling(init), q, hcompressScaling<T>())(fptr, init);
  return true;
}

template <typename T>
bool CompressAptly::plio(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {

  constexpr auto bp = bitpix<T>();

  // Float or too large int
  if constexpr (bp < 0 || bp > 32) {
    return false;
  }

  // Maybe
  if constexpr (bp > 16) {
    if (not init.data) {
      return false;
    }
    const auto max = *std::max_element(init.data, init.data + shapeSize(init.shape));
    if (max >= (T(1) << 24)) {
      return false;
    }
  }

  // OK
  Compress<Plio>(tiling(init))(fptr, init);
  return true;
}

template <typename T>
Quantization CompressAptly::quantization() const {
  if constexpr (std::is_integral_v<T>) {
    if (m_type != Type::Lossy) {
      return Quantization(0);
    }
    Quantization q(Tile::rms / 4);
    q.dithering(Quantization::Dithering::NonZeroPixel); // Keep nulls for integers
    return q;
  } else {
    if (m_type == Type::Lossless) {
      return Quantization(0);
    }
    return Quantization(Tile::rms / 16); // More conservative, imcopy default
  }
}

template <typename T>
Position<-1> CompressAptly::tiling(const ImageHdu::Initializer<T>& init) const {

  static constexpr long minSize = 1024 * 1024;
  if (shapeSize(init.shape) <= minSize) {
    return Tile::whole();
  }

  const long rowWidth = init.shape[0];
  const long rowSize = rowWidth * sizeof(T);
  const long rowCount = minSize / rowSize + 1;

  // FIXME reach minSize using higher dimensions

  return {rowWidth, rowCount};
}

template <typename T>
Position<-1> CompressAptly::hcompressTiling(const ImageHdu::Initializer<T>& init) const {

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
Scaling CompressAptly::hcompressScaling() const {
  if constexpr (std::is_integral_v<T>) {
    if (m_type != Type::Lossy) {
      return 0;
    }
    return Tile::rms * 2.5;
  } else {
    if (m_type == Type::Lossless) {
      return 0;
    }
    return Tile::rms * 2.5;
  }
}

} // namespace Fits
} // namespace Euclid

#endif
