// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/EleFitsBenchmark.h"

#if defined(_ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL) || defined(CHECK_QUALITY)

namespace Euclid {
namespace Fits {
namespace Validation {

template <Linx::Index I>
TypedKey<typename std::tuple_element<I, BColumns>::type::Value, Linx::Index>
EleFitsColwiseBenchmark::col_indexed() const
{
  return TypedKey<typename std::tuple_element<I, BColumns>::type::Value, Linx::Index>(I);
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
