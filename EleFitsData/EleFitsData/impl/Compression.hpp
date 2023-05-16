// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {
namespace Compression {

// TODO verify default values
Quantification::Quantification() : m_level(0.f), m_dither(Dithering::EveryPixelDithering), m_lossyInt(false) {}

void Quantification::absoluteLevel(float qlevel) {

  if (qlevel < 0.f)
    throw FitsError("Absolute quantize level out of supported bounds");

  m_level = -qlevel; // absoluteness stored internally as negative value like in cfitsio
}

void Quantification::relativeLevel(float qlevel) {

  if (qlevel < 0.f)
    throw FitsError("Relative quantize level out of supported bounds");

  m_level = qlevel;
}

void Quantification::dither(Dithering dither) {
  m_dither = std::move(dither);
}

void Quantification::enableLossyInt() {
  m_lossyInt = true;
}

void Quantification::disableLossyInt() {
  m_lossyInt = false;
}

float Quantification::level() const {
  return std::abs(m_level);
}

bool Quantification::isAbsolute() const {
  return m_level < 0.f;
}

Dithering Quantification::dither() const {
  return m_dither;
}

bool Quantification::hasLossyInt() const {
  return m_lossyInt;
}

Scale::Scale(float factor) : m_factor(factor) {}

Scale Scale::absolute(float factor) {

  if (factor < 0.f)
    throw FitsError("Absolute scale factor out of supported bounds");

  return Scale(-factor); // absoluteness stored internally as negative value like in cfitsio
}

Scale Scale::relativeToNoise(float factor) {

  if (factor < 0.f)
    throw FitsError("Relative scale factor out of supported bounds");

  return Scale(factor);
}

float Scale::factor() const {
  return std::abs(m_factor);
}

bool Scale::isAbsolute() const {
  return m_factor < 0.f;
}

template <long N, typename TDerived>
AlgoMixin<N, TDerived>::AlgoMixin(Position<N> shape) : m_shape(std::move(shape)), m_quantize(Quantification()) {
  static_assert(N >= 0 and N <= 6, "N must be positive and less or equal to 6");
}

template <long N, typename TDerived>
void AlgoMixin<N, TDerived>::quantize(Quantification quantize) {
  m_quantize = std::move(quantize);
}

template <long N, typename TDerived>
const Position<N>& AlgoMixin<N, TDerived>::shape() const {
  return m_shape;
}

template <long N, typename TDerived>
const Quantification& AlgoMixin<N, TDerived>::quantize() const {
  return m_quantize;
}

None::None() : AlgoMixin<0, None>(Position<0>()) {}

template <long N>
Rice<N>::Rice(const Position<N> shape) : AlgoMixin<N, Rice<N>>(shape) {}

template <long N>
HCompress<N>::HCompress(const Position<N> shape) :
    AlgoMixin<N, HCompress<N>>(shape), m_scale(Scale::relativeToNoise(0.f)), m_smooth(false) {}

template <long N>
void HCompress<N>::scale(Scale scale) {
  m_scale = std::move(scale);
}

template <long N>
void HCompress<N>::enableSmoothing() {
  m_smooth = true;
}

template <long N>
void HCompress<N>::disableSmoothing() {
  m_smooth = false;
}

template <long N>
const Scale& HCompress<N>::scale() const {
  return m_scale;
}

template <long N>
bool HCompress<N>::isSmooth() const {
  return m_smooth;
}

template <long N>
Plio<N>::Plio(const Position<N> shape) : AlgoMixin<N, Plio<N>>(shape) {}

template <long N>
Gzip<N>::Gzip(const Position<N> shape) : AlgoMixin<N, Gzip<N>>(shape) {}

template <long N>
ShuffledGzip<N>::ShuffledGzip(const Position<N> shape) : AlgoMixin<N, ShuffledGzip<N>>(shape) {}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

#endif