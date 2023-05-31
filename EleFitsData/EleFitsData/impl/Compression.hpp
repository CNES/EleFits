// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {
namespace Compression {

Factor::Factor(float value) : m_value(value) {}

Factor Factor::none() {
  return Factor(0.f);
}

Factor Factor::absolute(float value) {

  if (value <= 0.f)
    throw FitsError("Absolute factor value out of supported bounds");

  return Factor(-value); // absoluteness stored internally as negative value like in cfitsio
}

Factor Factor::relative(float value) {

  if (value <= 0.f)
    throw FitsError("Relative factor value out of supported bounds");

  return Factor(value);
}

Factor::Type Factor::type() const {
  if (m_value > 0.f) {
    return Factor::Type::Relative;
  } else if (m_value < 0.f) {
    return Factor::Type::Absolute;
  } else {
    return Factor::Type::None;
  }
}

float Factor::value() const {
  return std::abs(m_value);
}

inline bool Factor::operator==(const Factor& f2) const {
  return (m_value == f2.m_value) and (this->type() == f2.type());
}

// TODO still have to verify default values for dithering and lossyInt
Quantization::Quantization() : m_level(Factor::none()), m_dithering(Dithering::EveryPixel), m_lossyInt(false) {}

Quantization& Quantization::level(Factor level) {
  m_level = std::move(level);
  return *this;
}

Quantization& Quantization::dithering(Dithering dither) {
  m_dithering = std::move(dither);
  return *this;
}

Quantization& Quantization::enableLossyInt() {
  m_lossyInt = true;
  return *this;
}

Quantization& Quantization::disableLossyInt() {
  m_lossyInt = false;
  return *this;
}

const Factor& Quantization::level() const {
  return m_level;
}

Dithering Quantization::dithering() const {
  return m_dithering;
}

bool Quantization::hasLossyInt() const {
  return m_lossyInt;
}

inline bool Quantization::operator==(const Quantization& q2) const {
  return (m_level == q2.level()) and (m_dithering == q2.dithering()) and (m_lossyInt == q2.hasLossyInt());
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
    AlgoMixin<2, HCompress>(shape), m_scale(Factor::none()), m_smooth(false) {}

void HCompress::scale(Factor scale) {
  m_scale = std::move(scale);
}

void HCompress::enableSmoothing() {
  m_smooth = true;
}

void HCompress::disableSmoothing() {
  m_smooth = false;
}

const Factor& HCompress::scale() const {
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