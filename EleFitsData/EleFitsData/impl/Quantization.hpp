// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_QUANTIZATION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Quantization.h"

namespace Euclid {
namespace Fits {

Quantization::Quantization() : Quantization(Scaling(0), Dithering::None) {}

Quantization::Quantization(Scaling level) : Quantization(level, level ? Dithering::EveryPixel : Dithering::None) {}

Quantization::Quantization(Scaling level, Dithering method) : m_level(std::move(level)), m_dithering(Dithering::None) {
  dithering(method); // Enables compatibility check
}

Quantization& Quantization::level(Scaling level) {
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

const Scaling& Quantization::level() const {
  return m_level;
}

Quantization::Dithering Quantization::dithering() const {
  return m_dithering;
}

Quantization::operator bool() const {
  return bool(m_level);
}

bool Quantization::operator==(const Quantization& rhs) const {
  return (m_level == rhs.m_level) && (m_dithering == rhs.m_dithering);
}

bool Quantization::operator!=(const Quantization& rhs) const {
  return not(*this == rhs);
}

} // namespace Fits
} // namespace Euclid

#endif