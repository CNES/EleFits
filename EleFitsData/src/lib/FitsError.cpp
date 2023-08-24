// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

const std::string FitsError::m_prefix = "EleFits error: ";

FitsError::FitsError(const std::string& message) : std::exception(), m_message(m_prefix + message) {}

OutOfBoundsError::OutOfBoundsError(const std::string& prefix, long value, std::pair<long, long> bounds) :
    FitsError(
        prefix + ": " + std::to_string(value) + " not in (" + std::to_string(bounds.first) + ", " +
        std::to_string(bounds.second) + ")") {}

void OutOfBoundsError::may_throw(const std::string& prefix, long value, std::pair<long, long> bounds) {
  if (value < bounds.first || value > bounds.second) {
    throw OutOfBoundsError(prefix, value, bounds);
  }
}

const char* FitsError::what() const noexcept {
  return m_message.c_str();
}

void FitsError::append(const std::string& text, std::size_t indent) {
  m_message += "\n";
  for (std::size_t i = 0; i < indent; ++i) {
    m_message += "  ";
  }
  m_message += text;
}

} // namespace Fits
} // namespace Euclid
