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

#include "EL_FitsData/Record.h"

namespace Euclid {
namespace FitsIO {

/*
 * 70 bytes minus opening and closing quotes
 */
constexpr std::size_t maxShortValueLength = 68;

template <>
bool Record<std::string>::hasLongStringValue() const {
  return value.length() > maxShortValueLength;
}

template <>
bool Record<const char *>::hasLongStringValue() const {
  return std::strlen(value) > maxShortValueLength;
}

template <>
bool Record<boost::any>::hasLongStringValue() const {
  const auto &id = value.type();
  if (id == typeid(std::string)) {
    return boost::any_cast<std::string>(value).length() > maxShortValueLength;
  }
  if (id == typeid(const char *)) {
    return std::strlen(boost::any_cast<const char *>(value)) > maxShortValueLength;
  }
  return false;
}

} // namespace FitsIO
} // namespace Euclid