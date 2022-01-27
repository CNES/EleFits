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

#include "EleFitsData/Raster.h"

namespace Euclid {
namespace Fits {

#ifndef COMPILE_RASTER_CLASSES
#define COMPILE_RASTER_CLASSES(type, unused) \
  template class Raster<type, -1, type*>; \
  template class Raster<type, 2, type*>; \
  template class Raster<type, 3, type*>; \
  template class Raster<const type, -1, const type*>; \
  template class Raster<const type, 2, const type*>; \
  template class Raster<const type, 3, const type*>; \
  template class Raster<type, -1, std::vector<type>>; \
  template class Raster<type, 2, std::vector<type>>; \
  template class Raster<type, 3, std::vector<type>>;
ELEFITS_FOREACH_RASTER_TYPE(COMPILE_RASTER_CLASSES)
#undef COMPILE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid
