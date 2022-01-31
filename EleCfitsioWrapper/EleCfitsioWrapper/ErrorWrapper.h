// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_ERRORWRAPPER_H
#define _ELECFITSIOWRAPPER_ERRORWRAPPER_H

#include "EleFitsData/FitsError.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief The error class which is thrown when an error occurs in a CFitsIO routine.
 */
class CfitsioError : public Fits::FitsError {

public:
  /**
   * @brief Destructor.
   */
  virtual ~CfitsioError() = default;

  /**
   * @brief Create from CFitsIO error code.
   * @details This generate the associated CFitsIO message with no context.
   */
  explicit CfitsioError(int cfitsioStatus);

  /**
   * @brief Create a CfitsioError.
   * @param cfitsioStatus The CFitsIO error status
   * @param fptr The fitsfile pointer to provide informations on the file
   * @param context A more human-friendly message to explain the error context
   * @details
   * Prints the context, then some file metadata, and then the CFitsIO error stack.
   */
  CfitsioError(int cfitsioStatus, fitsfile* fptr, const std::string& context);

  /**
   * @brief Get the error message of an error code.
   */
  static std::string message(int cfitsioStatus);

  /**
   * @brief Throw a CfitsioError if `cfitsioStatus > 0`.
   */
  static void mayThrow(int cfitsioStatus);

  /**
   * @brief Throw a CfitsioError if `cfitsioStatus > 0`.
   */
  static void mayThrow(int cfitsioStatus, fitsfile* fptr, const std::string& context);

public:
  /**
   * @brief The CFitsIO error code.
   */
  int status;
};

/**
 * @brief Check whether the file is valid and throw an error if not.
 */
void mayThrowInvalidFileError(fitsfile* fptr);

/**
 * @brief Check whether the file is writable and throw an error if not.
 */
void mayThrowReadonlyError(fitsfile* fptr);

} // namespace Cfitsio
} // namespace Euclid

#endif
