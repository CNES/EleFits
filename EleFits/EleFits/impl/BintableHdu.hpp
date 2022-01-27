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

#if defined(_ELEFITS_BINTABLEHDU_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/BintableHdu.h"

#include <algorithm>

namespace Euclid {
namespace Fits {

template <typename T, long N>
VecColumn<T, N> BintableHdu::readColumn(ColumnKey key) const {
  return m_columns.read<T, N>(std::move(key));
}

template <typename TColumn>
void BintableHdu::writeColumn(const TColumn& column) const {
  m_columns.write(column);
}

/**
 * @brief Specialization of `Hdu::as()` for the data unit.
 */
template <>
const BintableColumns& Hdu::as() const;

#ifndef DECLARE_READ_COLUMN
#define DECLARE_READ_COLUMN(T, _) extern template VecColumn<T, 1> BintableHdu::readColumn(ColumnKey) const;
ELEFITS_FOREACH_COLUMN_TYPE(DECLARE_READ_COLUMN)
#undef DECLARE_READ_COLUMN
#endif

#ifndef DECLARE_WRITE_COLUMN
#define DECLARE_WRITE_COLUMN(T, _) \
  extern template void BintableHdu::writeColumn(const PtrColumn<T, 1>&) const; \
  extern template void BintableHdu::writeColumn(const PtrColumn<const T, 1>&) const; \
  extern template void BintableHdu::writeColumn(const VecColumn<T, 1>&) const;
ELEFITS_FOREACH_COLUMN_TYPE(DECLARE_WRITE_COLUMN)
#undef DECLARE_WRITE_COLUMN
#endif

} // namespace Fits
} // namespace Euclid

#endif
