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

#include "EleFitsValidation/CsvAppender.h"

namespace Euclid {
namespace Fits {
namespace Validation {

CsvAppender::CsvAppender(const std::string& filename, const std::vector<std::string>& header, const std::string& sep) :
    m_file(filename, std::ios::out | std::ios::app), m_sep(sep) {
  if (header.empty()) {
    return;
  }
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  const bool append = in.tellg(); // position is 0 if file was just created
  if (append) {
    // TODO check header consistency
  } else {
    for (const auto& h : header) {
      (*this) << h;
    }
    (*this) << std::endl;
  }
}

CsvAppender& CsvAppender::operator<<(std::ostream& (*pf)(std::ostream&)) {
  m_file << pf;
  return *this;
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid
