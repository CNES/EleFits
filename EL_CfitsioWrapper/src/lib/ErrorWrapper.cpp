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

#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"

namespace Euclid {
namespace Cfitsio {

CfitsioError::CfitsioError(int cfitsioStatus) : FitsIOError(cfitsioErrorMessage(cfitsioStatus)), status(cfitsioStatus) {
}

CfitsioError::CfitsioError(int cfitsioStatus, const std::string &context) :
    FitsIOError(cfitsioErrorMessage(cfitsioStatus) + " (" + context + ")"),
    status(cfitsioStatus) {
}

std::string cfitsioErrorMessage(int status) {
  char cfitsio_msg[FLEN_ERRMSG];
  cfitsio_msg[0] = '\0';
  fits_get_errstatus(status, cfitsio_msg);
  std::string error_msg = "CFitsIO error " + std::to_string(status) + ": " + cfitsio_msg;
  return error_msg;
}

void mayThrowCfitsioError(int status, const std::string &context) {
  if (status == 0) {
    return;
  }
  if (context.empty()) {
    throw CfitsioError(status);
  } else {
    throw CfitsioError(status, context);
  }
}

void mayThrowReadonlyError(fitsfile *fptr) {
  if (not File::isWritable(fptr)) {
    mayThrowCfitsioError(READONLY_FILE);
  }
}

void mayThrowInvalidFileError(fitsfile *fptr) {
  if (not fptr) {
    mayThrowCfitsioError(BAD_FILEPTR);
  }
}

} // namespace Cfitsio
} // namespace Euclid
