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
#include <utility>

namespace Euclid {
namespace FitsIO {

/**
 * @brief The base exception which is thrown by the library.
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

  /**
   * @brief Append some line to the message.
   * @param line The line to be appended
   * @param indent Some indentation level
   */
  void append(const std::string &line, std::size_t indent = 0);

private:
  static const std::string m_prefix;
  std::string m_message;
};

/**
 * @brief The exception which is thrown if a value lies out of given bounds.
 */
class OutOfBoundsError : public FitsIOError {
public:
  /**
   * @brief Constructor.
   * @details
   * The error message is of the form "<prefix>: <value> not in (<min>, <max>)".
   */
  OutOfBoundsError(const std::string &prefix, long value, std::pair<long, long> bounds);

  /**
   * @brief Throw if a value lies out of given bounds, included.
   */
  static void mayThrow(const std::string &prefix, long value, std::pair<long, long> bounds);
};

} // namespace FitsIO
} // namespace Euclid

#endif
