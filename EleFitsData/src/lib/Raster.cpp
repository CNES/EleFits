// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Raster.h"

namespace Euclid {
namespace Fits {

#ifndef COMPILE_RASTER_CLASSES
#define COMPILE_RASTER_CLASSES(T, unused) \
  template class Raster<T, -1, DataContainerHolder<T, T*>>; \
  template class Raster<T, 2, DataContainerHolder<T, T*>>; \
  template class Raster<T, 3, DataContainerHolder<T, T*>>; \
  template class Raster<const T, -1, DataContainerHolder<const T, const T*>>; \
  template class Raster<const T, 2, DataContainerHolder<const T, const T*>>; \
  template class Raster<const T, 3, DataContainerHolder<const T, const T*>>; \
  template class Raster<T, -1, DataContainerHolder<T, std::vector<T>>>; \
  template class Raster<T, 2, DataContainerHolder<T, std::vector<T>>>; \
  template class Raster<T, 3, DataContainerHolder<T, std::vector<T>>>;
ELEFITS_FOREACH_RASTER_TYPE(COMPILE_RASTER_CLASSES)
#undef COMPILE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid
