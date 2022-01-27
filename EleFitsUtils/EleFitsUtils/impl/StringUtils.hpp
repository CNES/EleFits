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

#if defined(_ELEFITSUTILS_STRINGUTILS_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsUtils/StringUtils.h"

#include <algorithm>

namespace Euclid {
namespace Fits {
namespace String {

template <typename T>
CStrArray::CStrArray(const T begin, const T end) : smartPtrVector(end - begin), cStrVector(end - begin) {

  /* Build the vector of smart pointers */
  std::transform(begin, end, smartPtrVector.begin(), [](const std::string& str) {
    auto ptr = std::make_unique<char[]>(str.length() + 1);
    std::strcpy(ptr.get(), str.c_str());
    return ptr;
  });

  /* Build the vector of raw pointers */
  std::transform(smartPtrVector.begin(), smartPtrVector.end(), cStrVector.begin(), [](std::unique_ptr<char[]>& ptr) {
    return ptr.get();
  });
}

} // namespace String
} // namespace Fits
} // namespace Euclid

#endif
