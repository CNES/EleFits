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

#ifndef _ELEFITS_VALIDATION_CSVAPPENDER_H
#define _ELEFITS_VALIDATION_CSVAPPENDER_H

#include <fstream>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {
namespace Validation {

/**
 * @brief A CSV writer which creates a new file or appends rows to an existing file.
 */
class CsvAppender {
public:
  /**
   * @brief Constructor.
   * @param header The file header
   * @param sep The column separator
   * @details
   * If the file exists and the header is provided, it should match the first row of the file.
   * If the file does not exist, the header is written.
   */
  explicit CsvAppender(
      const std::string& filename,
      const std::vector<std::string>& header = {},
      const std::string& sep = "\t");

  /**
   * @brief Write a value.
   */
  template <typename T>
  CsvAppender& operator<<(const T& value);

  /**
   * @brief Apply a manipulator, e.g. `std::endl`.
   */
  CsvAppender& operator<<(std::ostream& (*pf)(std::ostream&));

  /**
   * @brief Write a row.
   */
  template <typename... Ts>
  CsvAppender& writeRow(const Ts&... values);

private:
  /** @brief The output stream. */
  std::ofstream m_file;

  /** @brief The column separator. */
  std::string m_sep;
};

} // namespace Validation
} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_VALIDATION_CSVAPPENDER_IMPL
#include "EleFitsValidation/impl/CsvAppender.hpp"
#undef _ELEFITS_VALIDATION_CSVAPPENDER_IMPL
/// @endcond

#endif
