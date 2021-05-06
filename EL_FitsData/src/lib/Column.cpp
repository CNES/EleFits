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

#include "EL_FitsData/Column.h"
#include "ElementsKernel/Unused.h"

namespace Euclid {
namespace FitsIO {

namespace Internal {

template <>
VecColumn<std::string>::VecColumn(ColumnInfo<std::string> info_, long rowCount) :
    Column<std::string>(info_),
    m_vec(rowCount) {
}

template <>
long rowCountDispatchImpl<std::string>(long elementCount, ELEMENTS_UNUSED long repeatCount) {
  return elementCount;
}

} // namespace Internal

#ifndef COMPILE_COLUMN_CLASSES
#define COMPILE_COLUMN_CLASSES(type, unused) \
  template struct ColumnInfo<type>; \
  template class Column<type>; \
  template class PtrColumn<type>; \
  template class VecRefColumn<type>; \
  template class VecColumn<type>;
EL_FITSIO_FOREACH_COLUMN_TYPE(COMPILE_COLUMN_CLASSES)
#undef COMPILE_COLUMN_CLASSES
#endif

} // namespace FitsIO
} // namespace Euclid
