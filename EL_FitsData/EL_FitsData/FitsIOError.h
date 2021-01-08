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

#ifndef _EL_FITSDATA_FITSIOERROR_H
#define _EL_FITSDATA_FITSIOERROR_H

#include <stdexcept>
#include <string>

namespace Euclid {
namespace FitsIO {

/**
 * @brief The error class which is thrown by the library.
 */
class FitsIOError : public std::exception {

public:
  /**
   * @brief Destructor.
   */
  virtual ~FitsIOError() = default;

  /**
   * @brief Constructor.
   * @param message Input message
   */
  explicit FitsIOError(const std::string &message);

  /**
   * @brief Output message.
   */
  const char *what() const noexcept override;

private:
  static const std::string m_prefix;
  std::string m_message;
};

} // namespace FitsIO
} // namespace Euclid

#endif
