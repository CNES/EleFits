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

#include "EleFitsUtils/StringUtils.h"

#include <boost/algorithm/string.hpp>

namespace Euclid {
namespace Fits {
namespace String {

std::vector<std::string> split(const std::string& input, const std::string& delimiters) {
  std::vector<std::string> res;
  boost::split(res, input, boost::is_any_of(delimiters));
  return res;
}

std::string trim(const std::string& input, const std::string& characters) {
  if (characters.length() > 0) {
    return boost::trim_copy_if(input, boost::is_any_of(characters));
  }
  return boost::trim_copy(input);
}

std::string readFile(const std::string& filename) {
  std::ifstream ifs(filename);
  return {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
}

std::string readAuxFile(const std::string& filename) {
  return readFile(Elements::getAuxiliaryPath(filename).string());
}

std::unique_ptr<char[]> toCharPtr(const std::string& str) {
  const long size = str.length();
  auto c_str = std::make_unique<char[]>(size + 1);
  strcpy(c_str.get(), str.c_str());
  return c_str;
}

CStrArray::CStrArray(const std::vector<std::string>& data) : CStrArray(data.begin(), data.end()) {}

CStrArray::CStrArray(const std::initializer_list<std::string>& data) : CStrArray(data.begin(), data.end()) {}

std::size_t CStrArray::size() const {
  return cStrVector.size();
}

char** CStrArray::data() {
  return cStrVector.data();
}

} // namespace String
} // namespace Fits
} // namespace Euclid
