// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_FITSIOERROR_H
#define _ELEFITSDATA_FITSIOERROR_H

#include "Linx/Base/TypeUtils.h" // Index

#include <stdexcept>
#include <string>
#include <utility>

namespace Fits {

/**
 * @ingroup exceptions
 * @brief Base of all exceptions thrown directly by the library.
 */
class FitsError : public std::exception {
public:

  /**
   * @brief Destructor.
   */
  virtual ~FitsError() = default;

  /**
   * @brief Constructor.
   * @param message Input message
   */
  explicit FitsError(const std::string& message);

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
 * @ingroup exceptions
 * @brief Exception thrown if a value lies out of given bounds.
 */
class OutOfBoundsError : public FitsError {
public:

  /**
   * @brief Constructor.
   * @details
   * The error message is of the form "<prefix>: <value> not in (<min>, <max>)".
   */
  OutOfBoundsError(const std::string& prefix, Linx::Index value, std::pair<Linx::Index, Linx::Index> bounds);

  /**
   * @brief Throw if a value lies out of given bounds, included.
   */
  static void may_throw(const std::string& prefix, Linx::Index value, std::pair<Linx::Index, Linx::Index> bounds);
};

/**
 * @ingroup exceptions
 * @brief Exception thrown if a checksum is missing or incorrect.
 */
struct ChecksumError : public FitsError {
  /**
   * @brief Status of a checksum stored in a header unit.
   */
  enum Status {
    Incorrect = -1, ///< Incorrect checksum value
    Missing = 0, ///< Missing checksum record
    Correct = 1 ///< Correct checksum value
  };

  /**
   * @brief Constructor.
   */
  ChecksumError(Status hdu_status, Status data_status) :
      FitsError("Checksum error: "), hdu(hdu_status), data(data_status)
  {
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
  bool missing() const
  {
    return (hdu == Missing) || (data == Missing);
  }

  /**
   * @brief Check whether at least one checksum is incorrect.
   */
  bool incorrect() const
  {
    return (hdu == Incorrect) || (data == Incorrect);
  }

  /**
   * @brief Throw if at least one checksum is not correct (missing or incorrect).
   */
  static void may_throw(Status hdu_status, Status data_status)
  {
    if (hdu_status != Correct || data_status != Correct) {
      throw ChecksumError(hdu_status, data_status);
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

} // namespace Fits

#endif
