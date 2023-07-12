// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

Compression::Scaling::Scaling(double value, Type type) : m_type(type), m_value(value) {
  if (value < 0) {
    throw FitsError("Scaling value must be positive or null");
  }
}

Compression::Scaling::operator bool() const {
  return m_value;
}

Compression::Scaling::Type Compression::Scaling::type() const {
  return m_type;
}

double Compression::Scaling::value() const {
  return m_value;
}

bool Compression::Scaling::operator==(const Compression::Scaling& rhs) const {

  // Both disabled
  if (not *this && not rhs) {
    return true;
  }

  // Same type
  if (m_type == rhs.m_type) {
    return m_value == rhs.m_value;
  }

  // One absolute and one relative
  if (m_type == Type::Absolute || rhs.m_type == Type::Absolute) {
    return false;
  }

  // One factor and one inverse
  return m_value == 1. / rhs.m_value;
}

bool Compression::Scaling::operator!=(const Compression::Scaling& rhs) const {
  return not(*this == rhs);
}

inline Compression::TileRms rms; // Definition

Compression::Quantization::Quantization() : Quantization::Quantization(Compression::Scaling(0), Dithering::None) {}

Compression::Quantization::Quantization(Compression::Scaling level) :
    Compression::Quantization::Quantization(level, level ? Dithering::EveryPixel : Dithering::None) {}

Compression::Quantization::Quantization(Compression::Scaling level, Compression::Dithering method) :
    m_level(std::move(level)), m_dithering(Dithering::None) {
  dithering(method); // Enables compatibility check
}

Compression::Quantization& Compression::Quantization::level(Compression::Scaling level) {
  m_level = std::move(level);
  if (not m_level) {
    m_dithering = Dithering::None;
  }
  return *this;
}

Compression::Quantization& Compression::Quantization::dithering(Dithering method) {
  if (not m_level && method != Dithering::None) {
    throw FitsError("Cannot set dithering method when quantization is deactivated");
  }
  m_dithering = std::move(method);
  return *this;
}

const Compression::Scaling& Compression::Quantization::level() const {
  return m_level;
}

Compression::Dithering Compression::Quantization::dithering() const {
  return m_dithering;
}

Compression::Quantization::operator bool() const {
  return m_level;
}

bool Compression::Quantization::operator==(const Compression::Quantization& rhs) const {
  return (m_level == rhs.m_level) && (m_dithering == rhs.m_dithering);
}

bool Compression::Quantization::operator!=(const Compression::Quantization& rhs) const {
  return not(*this == rhs);
}

std::unique_ptr<Compression> Compression::makeLosslessAlgo(long bitpix, long dimension) {

  // No data
  if (dimension == 0) {
    return std::make_unique<NoCompression>();
  }

  // Floating point
  if (bitpix < 0) {
    return std::make_unique<ShuffledGzip>(); // FIXME Gzip?
  }

  // Mask
  if (bitpix <= 24) {
    return std::make_unique<Plio>();
  }

  // 2D or more
  if (dimension >= 2) {
    return std::make_unique<HCompress>();
  }

  // Fallback
  return std::make_unique<Rice>();
}

std::unique_ptr<Compression> Compression::makeAlgo(long bitpix, long dimension) {

  // No data
  if (dimension == 0) {
    return std::make_unique<NoCompression>();
  }

  // Mask
  if (bitpix > 0 && bitpix <= 24) {
    return std::make_unique<Plio>();
  }

  const auto q4 = Compression::Quantization(Compression::rms / 4);

  // 2D or more
  if (dimension >= 2) {
    auto out = std::make_unique<HCompress>();
    out->quantization(std::move(q4));
    out->scaling(Compression::rms * 2.5);
    return out;
  }

  // Fallback
  auto out = std::make_unique<Rice>();
  out->quantization(std::move(q4));
  return out;
}

template <typename TRaster>
std::unique_ptr<Compression> Compression::makeLosslessAlgo(const TRaster& raster) {

  // No data
  if (raster.size() == 0) {
    return std::make_unique<NoCompression>();
  }

  // Mask
  const auto b = bitpix(raster);
  const auto n = raster.dimension();
  if (b > 0 && raster.data()) {
    const auto max = *std::max_element(raster.begin(), raster.end());
    if (max < (1 << 24)) {
      return std::make_unique<Plio>();
    }
  }

  // Fallback
  return makeLosslessAlgo(b, n);

  // FIXME HCompress needs tiles of at least 4x4
  // And, according to astropy:
  // # If the image has less than 30
  // # rows, then the entire image will be compressed as a single
  // # tile.  Otherwise the tiles will consist of 16 rows of the
  // # image.  This keeps the tiles to a reasonable size, and it
  // # also includes enough rows to allow good compression
  // # efficiency.  It the last tile of the image happens to contain
  // # less than 4 rows, then find another tile size with between 14
  // # and 30 rows (preferably even), so that the last tile has at
  // # least 4 rows.

  // FIXME increase tiling if width or size is small
}

template <typename TRaster>
std::unique_ptr<Compression> Compression::makeAlgo(const TRaster& raster) {
  const auto b = bitpix(raster);
  if (b > 0) {
    return makeLosslessAlgo(raster);
  }
  return makeAlgo(b, raster.dimension());
}

Compression::Compression(Position<-1> tiling, Quantization quantization) :
    m_tiling(std::move(tiling)), m_quantization(std::move(quantization)) {
  OutOfBoundsError::mayThrow("Tiling dimension error", m_tiling.size(), {0, 6});
}

const Position<-1>& Compression::tiling() const {
  return m_tiling;
}

const Compression::Quantization& Compression::quantization() const {
  return m_quantization;
}

bool Compression::isLossless() const {
  return not m_quantization;
}

template <typename TDerived>
TDerived& AlgoMixin<TDerived>::tiling(Position<-1> tiling) {
  m_tiling = std::move(tiling);
  return static_cast<TDerived&>(*this);
}

template <typename TDerived>
TDerived& AlgoMixin<TDerived>::quantization(Quantization quantization) {
  m_quantization = std::move(quantization);
  return static_cast<TDerived&>(*this);
}

template <typename TDerived>
AlgoMixin<TDerived>::AlgoMixin(Position<-1> tiling, Quantization quantization) :
    Compression(std::move(tiling), std::move(quantization)) {}

NoCompression::NoCompression() : AlgoMixin<NoCompression>({}, Quantization()) {}

NoCompression& NoCompression::tiling(Position<-1>) {
  throw FitsError("Cannot set tiling for diabled compression");
  return *this;
}

NoCompression& NoCompression::quantization(Quantization) {
  throw FitsError("Cannot set quantization for diabled compression");
  return *this;
}

Gzip::Gzip(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<Gzip>(std::move(tiling), std::move(quantization)) {}

ShuffledGzip::ShuffledGzip(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<ShuffledGzip>(std::move(tiling), std::move(quantization)) {}

Rice::Rice(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<Rice>(std::move(tiling), std::move(quantization)) {}

HCompress::HCompress(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<HCompress>(std::move(tiling), std::move(quantization)), m_scale(Compression::Scaling(0)),
    m_smooth(false) {
  this->quantization(std::move(quantization));
}

bool HCompress::isLossless() const {
  if (m_scale) {
    return false;
  }
  return Compression::isLossless();
}

const Compression::Scaling& HCompress::scaling() const {
  return m_scale;
}

bool HCompress::isSmooth() const {
  return m_smooth;
}

HCompress& HCompress::scaling(Compression::Scaling scale) {
  m_scale = std::move(scale);
  return *this;
}

HCompress& HCompress::quantization(Quantization quantization) {
  if (quantization.dithering() == Dithering::NonZeroPixel) {
    throw FitsError("H-compress does not support non-zero pixel dithering");
  }
  AlgoMixin<HCompress>::quantization(quantization);
  return *this;
}

HCompress& HCompress::enableSmoothing() {
  m_smooth = true;
  return *this;
}

HCompress& HCompress::disableSmoothing() {
  m_smooth = false;
  return *this;
}

Plio::Plio(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<Plio>(std::move(tiling), std::move(quantization)) {}

} // namespace Fits
} // namespace Euclid

#endif