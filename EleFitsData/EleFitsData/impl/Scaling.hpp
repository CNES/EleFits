// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_SCALING_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Scaling.h"

namespace Euclid {
namespace Fits {

Scaling::Scaling() : Scaling(1, Type::Factor) {}

Scaling::Scaling(double value, Type type) : m_type(type), m_value(value) {
  if (value < 0) {
    throw FitsError("Scaling value must be positive or null");
  }
}

Scaling::operator bool() const {
  return m_value;
}

bool Scaling::is_identity() const {
  if (m_type == Type::Absolute) {
    return false;
  }
  return m_value == 1.;
}

Scaling::Type Scaling::type() const {
  return m_type;
}

double Scaling::value() const {
  return m_value;
}

bool Scaling::operator==(const Scaling& rhs) const {

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

bool Scaling::operator!=(const Scaling& rhs) const {
  return not(*this == rhs);
}

Scaling& Scaling::operator*=(double value) {
  if (value <= 0) {
    throw FitsError("Scaling multiplication requires positive value");
  }
  if (is_identity()) {
    m_type = Type::Factor;
    m_value = value;
  } else if (m_type == Type::Absolute || m_type == Type::Factor) {
    m_value *= value;
  } else {
    m_value /= value;
  }
  return *this;
}

Scaling& Scaling::operator/=(double value) {
  if (value <= 0) {
    throw FitsError("Scaling division requires positive value");
  }
  if (is_identity()) {
    m_type = Type::Inverse;
    m_value = value;
  } else if (m_type == Type::Absolute || m_type == Type::Factor) {
    m_value /= value;
  } else {
    m_value *= value;
  }
  return *this;
}

Scaling Scaling::operator*(double value) const {
  Scaling out = *this;
  out *= value;
  return out;
}

Scaling Scaling::operator/(double value) {
  Scaling out = *this;
  out /= value;
  return out;
}

} // namespace Fits
} // namespace Euclid

#endif