// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_COMPRESSIONSTRATEGY_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/CompressionStrategy.h"
#include "EleFitsData/DataUtils.h"

namespace Euclid {
namespace Fits {

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
std::unique_ptr<Compress<ShuffledGzip>> CompressAuto::gzip(const ImageHdu::Initializer<T>& init)
{
  return std::make_unique<Compress<ShuffledGzip>>(Tile::adaptive(), quantization<T>());
}

template <typename T>
std::unique_ptr<Compress<Rice>> CompressAuto::rice(const ImageHdu::Initializer<T>& init)
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
std::unique_ptr<Compress<Plio>> CompressAuto::plio(const ImageHdu::Initializer<T>& init)
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
