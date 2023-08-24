// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

inline Scaling Tile::rms; // Definition

Compression::Compression(Position<-1> tiling, Quantization quantization) :
    m_tiling(std::move(tiling)), m_quantization(std::move(quantization))
{
  OutOfBoundsError::may_throw("Tiling dimension error", m_tiling.size(), {0, 6});
}

const Position<-1>& Compression::tiling() const
{
  return m_tiling;
}

const Quantization& Compression::quantization() const
{
  return m_quantization;
}

bool Compression::is_lossless() const
{
  return not m_quantization;
}

template <typename TDerived>
TDerived& AlgoMixin<TDerived>::tiling(Position<-1> tiling)
{
  m_tiling = std::move(tiling);
  return static_cast<TDerived&>(*this);
}

template <typename TDerived>
TDerived& AlgoMixin<TDerived>::quantization(Quantization quantization)
{
  m_quantization = std::move(quantization);
  return static_cast<TDerived&>(*this);
}

template <typename TDerived>
AlgoMixin<TDerived>::AlgoMixin(Position<-1> tiling, Quantization quantization) :
    Compression(std::move(tiling), std::move(quantization))
{}

NoCompression::NoCompression() : AlgoMixin<NoCompression>({0}, Quantization()) {}

NoCompression& NoCompression::tiling(Position<-1>)
{
  throw FitsError("Cannot set tiling for disabled compression");
  return *this;
}

NoCompression& NoCompression::quantization(Quantization)
{
  throw FitsError("Cannot set quantization for disabled compression");
  return *this;
}

Gzip::Gzip(Position<-1> tiling, Quantization quantization) : AlgoMixin<Gzip>(std::move(tiling), std::move(quantization))
{}

ShuffledGzip::ShuffledGzip(Position<-1> tiling, Quantization quantization) :
    AlgoMixin<ShuffledGzip>(std::move(tiling), std::move(quantization))
{}

Rice::Rice(Position<-1> tiling, Quantization quantization) : AlgoMixin<Rice>(std::move(tiling), std::move(quantization))
{}

HCompress::HCompress(Position<-1> tiling, Quantization quantization, Scaling scaling) :
    AlgoMixin<HCompress>(std::move(tiling), std::move(quantization)), m_scale(std::move(scaling)), m_smooth(false)
{
  this->quantization(std::move(quantization));
}

bool HCompress::is_lossless() const
{
  if (m_scale) {
    return false;
  }
  return Compression::is_lossless();
}

const Scaling& HCompress::scaling() const
{
  return m_scale;
}

bool HCompress::is_smooth() const
{
  return m_smooth;
}

HCompress& HCompress::scaling(Scaling scale)
{
  m_scale = std::move(scale);
  return *this;
}

HCompress& HCompress::quantization(Quantization quantization)
{
  if (quantization.dithering() == Quantization::Dithering::NonZeroPixel) {
    throw FitsError("H-compress does not support non-zero pixel dithering");
  }
  AlgoMixin<HCompress>::quantization(quantization);
  return *this;
}

HCompress& HCompress::enable_smoothing()
{
  m_smooth = true;
  return *this;
}

HCompress& HCompress::disable_smoothing()
{
  m_smooth = false;
  return *this;
}

Plio::Plio(Position<-1> tiling, Quantization quantization) : AlgoMixin<Plio>(std::move(tiling), std::move(quantization))
{}

} // namespace Fits
} // namespace Euclid

#endif