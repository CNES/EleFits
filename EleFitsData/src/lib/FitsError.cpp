/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace FitsIO {

const std::string FitsError::m_prefix = "EL_FitsIO error: ";

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

} // namespace FitsIO
} // namespace Euclid
