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

/**
 * @brief Read a text file from the auxiliary directory.
 * @param filename The path to the file, relative to the auxiliary directory
 */
std::string readAuxFile(const std::string& filename) {
  return readFile(Elements::getAuxiliaryPath(filename).string());
}

} // namespace String
} // namespace Fits
} // namespace Euclid
