// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSUTILS_STRINGUTILS_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsUtils/StringUtils.h"

#include <algorithm>

namespace Euclid {
namespace Fits {
namespace String {

template <typename T>
CStrArray::CStrArray(const T begin, const T end) : smart_ptr_vector(end - begin), c_str_vector(end - begin) {

  /* Build the vector of smart pointers */
  std::transform(begin, end, smart_ptr_vector.begin(), [](const std::string& str) {
    auto ptr = std::make_unique<char[]>(str.length() + 1);
    std::strcpy(ptr.get(), str.c_str());
    return ptr;
  });

  /* Build the vector of raw pointers */
  std::transform(smart_ptr_vector.begin(), smart_ptr_vector.end(), c_str_vector.begin(), [](std::unique_ptr<char[]>& ptr) {
    return ptr.get();
  });
}

} // namespace String
} // namespace Fits
} // namespace Euclid

#endif
