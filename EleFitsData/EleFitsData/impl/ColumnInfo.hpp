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

#if defined(_ELEFITSDATA_COLUMNINFO_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/ColumnInfo.h"

namespace Euclid {
namespace Fits {

template <typename T, long N>
constexpr long ColumnInfo<T, N>::Dim;

template <typename T, long N>
long ColumnInfo<T, N>::repeatCount() const {
  return shapeSize(shape);
}

template <typename T, long N>
long ColumnInfo<T, N>::elementCount() const {
  if (std::is_same<T, std::string>::value) {
    return 1;
  }
  return repeatCount();
}

template <typename T, long N>
bool operator==(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return lhs.name == rhs.name && lhs.unit == rhs.unit && lhs.shape == rhs.shape;
}

template <typename T, long N>
bool operator!=(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs) {
  return not(lhs == rhs);
}

} // namespace Fits
} // namespace Euclid

#endif
