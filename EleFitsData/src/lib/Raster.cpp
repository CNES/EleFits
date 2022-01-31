// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
