// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {
namespace Compression {

// TODO still have to verify default values for dithering and lossyInt
Quantization::Quantization() : m_level(0.f), m_dithering(Dithering::EveryPixel), m_lossyInt(false) {}

void Quantization::absoluteLevel(float qlevel) {

  if (qlevel < 0.f)
    throw FitsError("Absolute quantize level out of supported bounds");

  m_level = -qlevel; // absoluteness stored internally as negative value like in cfitsio
}

void Quantization::relativeLevel(float qlevel) {

  if (qlevel < 0.f)
    throw FitsError("Relative quantize level out of supported bounds");

  m_level = qlevel;
}

void Quantization::dithering(Dithering dither) {
  m_dithering = std::move(dither);
}

void Quantization::enableLossyInt() {
  m_lossyInt = true;
}

void Quantization::disableLossyInt() {
  m_lossyInt = false;
}

float Quantization::level() const {
  return std::abs(m_level);
}

bool Quantization::isAbsolute() const {
  return m_level < 0.f;
}

Dithering Quantization::dithering() const {
  return m_dithering;
}

bool Quantization::hasLossyInt() const {
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
AlgoMixin<N, TDerived>::AlgoMixin(Position<N> shape) : m_shape(std::move(shape)), m_quantize(Quantization()) {
  static_assert(N >= 0 and N <= 6, "N must be positive and less or equal to 6");
}

template <long N, typename TDerived>
void AlgoMixin<N, TDerived>::quantize(Quantization quantize) {
  m_quantize = std::move(quantize);
}

template <long N, typename TDerived>
const Position<N>& AlgoMixin<N, TDerived>::shape() const {
  return m_shape;
}

template <long N, typename TDerived>
const Quantization& AlgoMixin<N, TDerived>::quantize() const {
  return m_quantize;
}

None::None() : AlgoMixin<0, None>(Position<0>()) {}

template <long N>
Rice<N>::Rice(const Position<N> shape) : AlgoMixin<N, Rice<N>>(shape) {}

HCompress::HCompress(const Position<2> shape) :
    AlgoMixin<2, HCompress>(shape), m_scale(Scale::relativeToNoise(0.f)), m_smooth(false) {}

void HCompress::scale(Scale scale) {
  m_scale = std::move(scale);
}

void HCompress::enableSmoothing() {
  m_smooth = true;
}

void HCompress::disableSmoothing() {
  m_smooth = false;
}

const Scale& HCompress::scale() const {
  return m_scale;
}

bool HCompress::isSmooth() const {
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