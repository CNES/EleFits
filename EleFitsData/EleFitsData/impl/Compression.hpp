// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Compression.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {
namespace Compression {

Factor Factor::none() {
  return Factor(0.f);
}

Factor Factor::absolute(float value) {
  if (value <= 0) {
    throw FitsError("Absolute factor value out of supported bounds");
  }
  return Factor(-value); // absoluteness stored internally as negative value like in cfitsio
}

Factor Factor::relative(float value) {
  if (value <= 0) {
    throw FitsError("Relative factor value out of supported bounds");
  }
  return Factor(value);
}

Factor::Type Factor::type() const {
  if (m_value > 0) {
    return Factor::Type::Relative;
  } else if (m_value < 0) {
    return Factor::Type::Absolute;
  } else {
    return Factor::Type::None;
  }
}

float Factor::value() const {
  return std::abs(m_value);
}

bool Factor::operator==(const Factor& rhs) const {
  return (m_value == rhs.m_value) && (this->type() == rhs.type());
}

Factor::Factor(float value) : m_value(value) {}

Quantization::Quantization(Factor level, Dithering method) :
    m_level(std::move(level)), m_dithering(method), m_lossyInt(false) {}

Quantization& Quantization::level(Factor level) {
  m_level = std::move(level);
  return *this;
}

Quantization& Quantization::dithering(Dithering method) {
  m_dithering = std::move(method);
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

bool Quantization::operator==(const Quantization& rhs) const {
  return (m_level == rhs.level()) && (m_dithering == rhs.dithering()) && (m_lossyInt == rhs.hasLossyInt());
}

template <long N, typename TDerived>
const Position<N>& AlgoMixin<N, TDerived>::shape() const {
  return m_shape;
}

template <long N, typename TDerived>
const Quantization& AlgoMixin<N, TDerived>::quantization() const {
  return m_quantization;
}

template <long N, typename TDerived>
TDerived& AlgoMixin<N, TDerived>::shape(Position<N> shape) {
  m_shape = std::move(shape);
  return dynamic_cast<TDerived&>(*this);
}

template <long N, typename TDerived>
TDerived& AlgoMixin<N, TDerived>::quantization(Quantization quantization) {
  m_quantization = std::move(quantization);
  return dynamic_cast<TDerived&>(*this);
}

template <long N, typename TDerived>
AlgoMixin<N, TDerived>::AlgoMixin(Position<N> shape) : m_shape(std::move(shape)), m_quantization(Quantization()) {
  static_assert(N == -1 || (N >= 0 && N <= 6), "N must be -1 or 6 at most");
}

None::None() : AlgoMixin<0, None>(Position<0>()) {}

template <long N>
Rice<N>::Rice(const Position<N> shape) : AlgoMixin<N, Rice<N>>(std::move(shape)) {}

HCompress::HCompress(const Position<2> shape) :
    AlgoMixin<2, HCompress>(std::move(shape)), m_scale(Factor::none()), m_smooth(false) {}

const Factor& HCompress::scale() const {
  return m_scale;
}

bool HCompress::isSmooth() const {
  return m_smooth;
}

HCompress& HCompress::scale(Factor scale) {
  m_scale = std::move(scale);
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

template <long N>
Plio<N>::Plio(Position<N> shape) : AlgoMixin<N, Plio<N>>(std::move(shape)) {}

template <long N>
Gzip<N>::Gzip(Position<N> shape) : AlgoMixin<N, Gzip<N>>(std::move(shape)) {}

template <long N>
ShuffledGzip<N>::ShuffledGzip(Position<N> shape) : AlgoMixin<N, ShuffledGzip<N>>(std::move(shape)) {}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

#endif