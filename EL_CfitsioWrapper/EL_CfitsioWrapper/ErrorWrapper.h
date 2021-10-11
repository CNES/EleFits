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

#ifndef _EL_CFITSIOWRAPPER_ERRORWRAPPER_H
#define _EL_CFITSIOWRAPPER_ERRORWRAPPER_H

#include "EL_FitsData/FitsError.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief The error class which is thrown when an error occurs in a CFitsIO routine.
 */
class CfitsioError : public FitsIO::FitsError {

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
