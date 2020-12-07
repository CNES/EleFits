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

#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Euclid {
namespace Cfitsio {

std::unique_ptr<char[]> toCharPtr(const std::string &str) {
  const long size = str.length();
  auto c_str = std::make_unique<char[]>(size + 1);
  strcpy(c_str.get(), str.c_str());
  return c_str;
}

CStrArray::CStrArray(const std::vector<std::string> &data) : CStrArray(data.begin(), data.end()) {
}

CStrArray::CStrArray(const std::initializer_list<std::string> &data) : CStrArray(data.begin(), data.end()) {
}

char **CStrArray::data() {
  return cStrVector.data();
}

} // namespace Cfitsio
} // namespace Euclid
