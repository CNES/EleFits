// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleFitsData/Compression.h"

namespace Euclid {
namespace Cfitsio {
namespace Compression {

void compress(fitsfile* fptr, Euclid::Fits::Compression::None algo);
template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Rice<N> algo);
template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::HCompress<N> algo);
template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Plio<N> algo);
template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Gzip<N> algo);
template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::ShuffledGzip<N> algo);

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

namespace Euclid {
namespace Fits {
namespace Compression {

Quantification::Quantification(float qlevel, FactorType qType) : m_level(qlevel), m_type(qType) {}

Quantification Quantification::absolute(float qlevel) {

  if (qlevel < 0.f)
    Euclid::Fits::FitsError("Absolute quantize level out of supported bounds");

  return Quantification(std::abs(qlevel), FactorType::Absolute);
}

Quantification Quantification::relativeToNoise(float qlevel) {

  if (qlevel < 0.f)
    Euclid::Fits::FitsError("Relative quantize level out of supported bounds");

  return Quantification(std::abs(qlevel), FactorType::Relative);
}

float Quantification::level() const {
  return this->m_level;
}

FactorType Quantification::type() const {
  return this->m_type;
}

Scale::Scale(float factor, FactorType sType) : m_factor(factor), m_type(sType) {}

Scale Scale::absolute(float factor) {

  if (factor < 0.f)
    Euclid::Fits::FitsError("Absolute scale factor out of supported bounds");

  return Scale(std::abs(factor), FactorType::Absolute);
}

Scale Scale::relativeToNoise(float factor) {

  if (factor < 0.f)
    Euclid::Fits::FitsError("Relative scale factor out of supported bounds");

  return Scale(std::abs(factor), FactorType::Relative);
}

float Scale::factor() const {
  return this->m_factor;
}

FactorType Scale::type() const {
  return this->m_type;
}

template <typename TDerived, long N>
AlgoMixin<TDerived, N>::AlgoMixin(const Euclid::Fits::Position<N> shape) : m_shape(std::move(shape)) {}

template <typename TDerived, long N>
void AlgoMixin<TDerived, N>::compress(fitsfile* fptr) const {

  int status = 0;

  Euclid::Fits::Position<N> ndims = this->m_shape;
  fits_set_tile_dim(fptr, N, ndims.data(), &status); // setting compression tile size
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  Euclid::Cfitsio::Compression::compress(fptr, static_cast<const TDerived&>(*this));
}

template <typename TDerived, long N>
FloatAlgo<TDerived, N>::FloatAlgo(const Euclid::Fits::Position<N> shape) :
    AlgoMixin<TDerived, N>(shape), m_quantize(Quantification::relativeToNoise(4.f)),
    m_dither(Dithering::EveryPixelDithering), m_lossyInt(false) {}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::set(Dithering dither) {
  this->m_dither = dither;
}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::set(Quantification quantize) {
  this->m_quantize = quantize;
}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::enableLossyInt() {
  this->m_lossyInt = true;
}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::disableLossyInt() {
  this->m_lossyInt = false;
}

template <typename TDerived, long N>
Dithering FloatAlgo<TDerived, N>::dither() const {
  return this->m_dither;
}

template <typename TDerived, long N>
Quantification FloatAlgo<TDerived, N>::quantize() const {
  return this->m_quantize;
}

template <typename TDerived, long N>
bool FloatAlgo<TDerived, N>::lossyInt() const {
  return this->m_lossyInt;
}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::compress(fitsfile* fptr) const {

  int status = 0;

  // setting quantize level:
  if (this->quantize().type() == FactorType::Relative) {
    fits_set_quantize_level(fptr, this->quantize().level(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative quantize level");
  } else { // absolute quantize level applied in this case
    fits_set_quantize_level(fptr, -this->quantize().level(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute quantize level");
  }

  // setting dither method:
  if (this->dither() == Dithering::NoDithering) {

    fits_set_quantize_dither(fptr, NO_DITHER, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NoDithering");

  } else if (this->dither() == Dithering::NonZeroPixelDithering) {

    fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_2, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NonZeroPixelDithering");

  } else { // Dithering::EveryPixelDithering in this case

    fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_1, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to EveryPixelDithering");
  }

  // setting lossy int compression:
  // TOFIX: how to verify that it is applied correctly to img?
  fits_set_lossy_int(fptr, this->lossyInt(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set lossy int compression");

  Euclid::Cfitsio::Compression::compress(fptr, static_cast<const TDerived&>(*this));
}

None::None() : AlgoMixin<None, 0>(this->none_shape) {}

template <long N>
Rice<N>::Rice(const Euclid::Fits::Position<N> shape) : FloatAlgo<Rice<N>, N>(shape) {}

template <long N>
HCompress<N>::HCompress(const Euclid::Fits::Position<N> shape) :
    FloatAlgo<HCompress<N>, N>(shape), m_scale(Scale::relativeToNoise(0.f)), m_smooth(false) {}

template <long N>
void HCompress<N>::set(Scale scale) {
  this->m_scale = scale; // TOFIX: make a copy ?
}

template <long N>
void HCompress<N>::enableSmoothing() {
  this->m_smooth = true;
}

template <long N>
void HCompress<N>::disableSmoothing() {
  this->m_smooth = false;
}

template <long N>
float HCompress<N>::scale() const {
  return this->m_scale.factor();
}

template <long N>
FactorType HCompress<N>::scaleType() const {
  return this->m_scale.type();
}

template <long N>
bool HCompress<N>::smooth() const {
  return this->m_smooth;
}

template <long N>
Plio<N>::Plio(const Euclid::Fits::Position<N> shape) : AlgoMixin<Plio<N>, N>(shape) {}

template <long N>
Gzip<N>::Gzip(const Euclid::Fits::Position<N> shape) : FloatAlgo<Gzip<N>, N>(shape) {}

template <long N>
ShuffledGzip<N>::ShuffledGzip(const Euclid::Fits::Position<N> shape) : FloatAlgo<ShuffledGzip<N>, N>(shape) {}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

#endif