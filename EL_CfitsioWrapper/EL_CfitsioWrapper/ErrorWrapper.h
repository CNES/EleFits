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

#include <string>

#include <fitsio.h>

#include "EL_FitsData/FitsIOError.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Runtime error printing CFitsIO messages.
 */
class CfitsioError : public FitsIO::FitsIOError {

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
   * @brief Create from CFitsIO error code and user-given context.
   * @details This generates the associated CFitsIO message with context.
   */
  CfitsioError(int cfitsioStatus, const std::string &context);

  /**
   * @brief The CFitsIO error code.
   */
  int status;
};

/**
 * @brief Get the error message of an error code.
 */
std::string cfitsioErrorMessage(int status);

/**
 * @brief Check whether status is OK (=0) and throw an error if not.
 */
void mayThrowCfitsioError(int status, const std::string &context = "");

/**
 * @brief Check whether the file is valid and throw an error if not.
 */
void mayThrowInvalidFileError(fitsfile *fptr);

/**
 * @brief Check whether the file is writable and throw an error if not.
 */
void mayThrowReadonlyError(fitsfile *fptr);

} // namespace Cfitsio
} // namespace Euclid

#endif
