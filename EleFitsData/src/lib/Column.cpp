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

#include "EleFitsData/Column.h"

namespace Euclid {
namespace Fits {

#ifndef COMPILE_COLUMN_CLASSES
#define COMPILE_COLUMN_CLASSES(type, unused) \
  template struct ColumnInfo<type, 1>; \
  template class Column<type, 1, type*>; \
  template class Column<const type, 1, const type*>; \
  template class Column<type, 1, std::vector<type>>;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_COLUMN_CLASSES)
#undef COMPILE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid
