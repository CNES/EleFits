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

#if defined(_ELEFITSDATA_COLUMNINFO_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/ColumnInfo.h"

namespace Euclid {
namespace Fits {

template <typename T, long N>
bool operator==(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return lhs.name == rhs.name && lhs.unit == rhs.unit && lhs.repeatCount == rhs.repeatCount;
}

template <typename T, long N>
bool operator!=(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return not(lhs == rhs);
}

/**
 * @brief String specialization.
 */
template <>
long FieldInfo<std::string>::elementCount() const;

template <typename T>
long FieldInfo<T>::elementCount() const {
  return repeatCount;
}

} // namespace Fits
} // namespace Euclid

#endif
