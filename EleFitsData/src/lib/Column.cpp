// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/Column.h"

namespace Euclid {
namespace Fits {

#ifndef COMPILE_COLUMN_CLASSES
#define COMPILE_COLUMN_CLASSES(T, unused) \
  template struct ColumnInfo<T, 1>; \
  template class Column<T, 1, DataContainerHolder<T, T*>>; \
  template class Column<const T, 1, DataContainerHolder<const T, const T*>>; \
  template class Column<T, 1, DataContainerHolder<T, std::vector<T>>>;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_COLUMN_CLASSES)
#undef COMPILE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid
