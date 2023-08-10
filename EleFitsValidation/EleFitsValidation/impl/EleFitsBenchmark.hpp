// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/EleFitsBenchmark.h"

#if defined(_ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL) || defined(CHECK_QUALITY)

namespace Euclid {
namespace Fits {
namespace Validation {

template <long i>
TypedKey<typename std::tuple_element<i, BColumns>::type::Value, long> EleFitsColwiseBenchmark::col_indexed() const {
  return TypedKey<typename std::tuple_element<i, BColumns>::type::Value, long>(i);
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
