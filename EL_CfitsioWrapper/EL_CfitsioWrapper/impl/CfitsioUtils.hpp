/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#if defined(_EL_CFITSIOWRAPPER_CFITSIOUTILS_IMPL) || defined(CHECK_QUALITY)

#include <algorithm>

#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Euclid {
namespace Cfitsio {

template <typename T>
CStrArray::CStrArray(const T begin, const T end) : smartPtrVector(end - begin), cStrVector(end - begin) {

  /* Build the vector of smart pointers */
  std::transform(begin, end, smartPtrVector.begin(), [](const std::string &str) {
    auto ptr = std::make_unique<char[]>(str.length() + 1);
    std::strcpy(ptr.get(), str.c_str());
    return ptr;
  });

  /* Build the vector of raw pointers */
  std::transform(smartPtrVector.begin(), smartPtrVector.end(), cStrVector.begin(), [](std::unique_ptr<char[]> &ptr) {
    return ptr.get();
  });
}

} // namespace Cfitsio
} // namespace Euclid

#endif
