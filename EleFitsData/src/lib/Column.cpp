// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
