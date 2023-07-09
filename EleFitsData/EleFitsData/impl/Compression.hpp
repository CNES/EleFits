// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

Param Param::none() {
  return Param(0);
}

Param Param::absolute(double value) {
  if (value <= 0) {
    throw FitsError("Absolute parameter value must be strictly positive");
  }
  return Param(-value); // Absoluteness stored internally as negative value like in FITS
}

Param Param::relative(double value) {
  if (value <= 0) {
    throw FitsError("Relative parameter value must be strictly positive");
  }
  return Param(value);
}

Param::Type Param::type() const {
  if (m_value > 0) {
    return Param::Type::Relative;
  } else if (m_value < 0) {
    return Param::Type::Absolute;
  } else {
    return Param::Type::None;
  }
}

double Param::value() const {
  return std::abs(m_value);
}

inline Param::operator bool() const {
  return m_value != 0;
}

bool Param::operator==(const Param& rhs) const {
  return m_value == rhs.m_value;
}

bool Param::operator!=(const Param& rhs) const {
  return not(*this == rhs);
}

Param::Param(double value) : m_value(value) {}

Quantization::Quantization() : Quantization::Quantization(Param::none(), Dithering::None) {}

Quantization::Quantization(Param level) :
    Quantization::Quantization(level, level ? Dithering::EveryPixel : Dithering::None) {}

Quantization::Quantization(Param level, Dithering method) : m_level(std::move(level)), m_dithering(Dithering::None) {
  dithering(method); // Enables compatibility check
}

Quantization& Quantization::level(Param level) {
  m_level = std::move(level);
  if (not m_level) {
    m_dithering = Dithering::None;
  }
  return *this;
}

Quantization& Quantization::dithering(Dithering method) {
  if (not m_level && method != Dithering::None) {
    throw FitsError("Cannot set dithering method when quantization is deactivated");
  }
  m_dithering = std::move(method);
  return *this;
}

const Param& Quantization::level() const {
  return m_level;
}

Dithering Quantization::dithering() const {
  return m_dithering;
}

Quantization::operator bool() const {
  return m_level;
}

bool Quantization::operator==(const Quantization& rhs) const {
  return (m_level == rhs.m_level) && (m_dithering == rhs.m_dithering);
}

bool Quantization::operator!=(const Quantization& rhs) const {
  return not(*this == rhs);
}

Compression::Compression(Position<-1> tiling, Quantization quantization) :
    m_tiling(std::move(tiling)), m_quantization(std::move(quantization)) {
  OutOfBoundsError::mayThrow("Tiling dimension error", m_tiling.size(), {0, 6});
}

const Position<-1>& Compression::tiling() const {
  return m_tiling;
}

const Quantization& Compression::quantization() const {
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
    AlgoMixin<HCompress>(std::move(tiling), std::move(quantization)), m_scale(Param::none()), m_smooth(false) {
  this->quantization(std::move(quantization));
}

bool HCompress::isLossless() const {
  if (m_scale) {
    return false;
  }
  return Compression::isLossless();
}

const Param& HCompress::scale() const {
  return m_scale;
}

bool HCompress::isSmooth() const {
  return m_smooth;
}

HCompress& HCompress::scale(Param scale) {
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