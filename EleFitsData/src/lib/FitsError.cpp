/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

const std::string FitsError::m_prefix = "EleFits error: ";

FitsError::FitsError(const std::string& message) : std::exception(), m_message(m_prefix + message) {}

OutOfBoundsError::OutOfBoundsError(const std::string& prefix, long value, std::pair<long, long> bounds) :
    FitsError(
        prefix + ": " + std::to_string(value) + " not in (" + std::to_string(bounds.first) + ", " +
        std::to_string(bounds.second) + ")") {}

void OutOfBoundsError::mayThrow(const std::string& prefix, long value, std::pair<long, long> bounds) {
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
