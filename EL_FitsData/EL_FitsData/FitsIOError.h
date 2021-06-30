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
 * @brief Base of all exceptions thrown directly by the library.
 * @ingroup exceptions
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
  explicit FitsIOError(const std::string& message);

  /**
   * @brief Output message.
   */
  const char* what() const noexcept override;

  /**
   * @brief Append some line to the message.
   * @param line The line to be appended
   * @param indent Some indentation level
   */
  void append(const std::string& line, std::size_t indent = 0);

private:
  static const std::string m_prefix;
  std::string m_message;
};

/**
 * @brief Exception thrown if a value lies out of given bounds.
 * @ingroup exceptions
 */
class OutOfBoundsError : public FitsIOError {
public:
  /**
   * @brief Constructor.
   * @details
   * The error message is of the form "<prefix>: <value> not in (<min>, <max>)".
   */
  OutOfBoundsError(const std::string& prefix, long value, std::pair<long, long> bounds);

  /**
   * @brief Throw if a value lies out of given bounds, included.
   */
  static void mayThrow(const std::string& prefix, long value, std::pair<long, long> bounds);
};

/**
 * @brief Exception thrown if a checksum is missing or incorrect.
 * @ingroup exceptions
 */
struct ChecksumError : public FitsIOError {

  /**
   * @brief Status of a checksum stored in a header unit.
   */
  enum Status
  {
    Incorrect = -1, ///< Incorrect checksum value
    Missing = 0, ///< Missing checksum record
    Correct = 1 ///< Correct checksum value
  };

  /**
   * @brief Constructor.
   */
  ChecksumError(Status hduStatus, Status dataStatus) :
      FitsIOError("Checksum error: "), hdu(hduStatus), data(dataStatus) {
    if (hdu == Missing) {
      append("Missing HDU checksum record. ");
    } else if (hdu == Incorrect) {
      append("Incorrect HDU checksum. ");
    }
    if (data == Missing) {
      append("Missing data checksum record. ");
    } else if (data == Incorrect) {
      append("Incorrect data checksum. ");
    }
  }

  /**
   * @brief Check whether at least one checksum is missing.
   */
  bool missing() const {
    return (hdu == Missing) || (data == Missing);
  }

  /**
   * @brief Check chether at least one checksum is incorrect.
   */
  bool incorrect() const {
    return (hdu == Incorrect) || (data == Incorrect);
  }

  /**
   * @brief Throw if at least one checksum is not correct (missing or incorrect).
   */
  static void mayThrow(Status hduStatus, Status dataStatus) {
    if (hduStatus != Correct || dataStatus != Correct) {
      throw ChecksumError(hduStatus, dataStatus);
    }
  }

  /**
   * @brief The checksum of the whole HDU.
   */
  Status hdu;

  /**
   * @brief The checksum of the data unit.
   */
  Status data;
};

} // namespace FitsIO
} // namespace Euclid

#endif
