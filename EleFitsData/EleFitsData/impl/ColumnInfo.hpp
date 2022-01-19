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

/// @cond
namespace Internal {

/**
 * @brief Compute the element count from the repeat count and value type.
 */
template <typename T>
long entryElementCountImpl(long repeatCount);

/**
 * @brief String specialization.
 */
template <>
long entryElementCountImpl<std::string>(long repeatCount);

template <typename T>
long entryElementCountImpl(long repeatCount) {
  return repeatCount;
}

} // namespace Internal
/// @endcond

template <typename T, long N>
long ColumnInfo<T, N>::elementCount() const {
  return Internal::entryElementCountImpl<T>(repeatCount);
}

template <typename T, long N>
bool operator==(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return lhs.name == rhs.name && lhs.unit == rhs.unit && lhs.repeatCount == rhs.repeatCount && lhs.shape == rhs.shape;
}

template <typename T, long N>
bool operator!=(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return not(lhs == rhs);
}

} // namespace Fits
} // namespace Euclid

#endif
