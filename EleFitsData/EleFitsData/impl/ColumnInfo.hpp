// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_COLUMNINFO_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/ColumnInfo.h"

namespace Euclid {
namespace Fits {

template <typename T, long N>
constexpr long ColumnInfo<T, N>::Dim;

template <typename T, long N>
long ColumnInfo<T, N>::repeat_count() const
{
  return shape_size(shape);
}

template <typename T, long N>
long ColumnInfo<T, N>::element_count() const
{
  if (std::is_same<T, std::string>::value) {
    return 1;
  }
  return repeat_count();
}

template <typename T, long N>
bool operator==(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs)
{
  return lhs.name == rhs.name && lhs.unit == rhs.unit && lhs.shape == rhs.shape;
}

template <typename T, long N>
bool operator!=(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs)
{
  return not(lhs == rhs);
}

} // namespace Fits
} // namespace Euclid

#endif
