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

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Cfitsio {

CfitsioError::CfitsioError(int cfitsioStatus) : FitsIOError(message(cfitsioStatus)), status(cfitsioStatus) {
}

CfitsioError::CfitsioError(int cfitsioStatus, fitsfile* fptr, const std::string& context) :
    FitsIOError(context),
    status(cfitsioStatus) {
  append("");
  if (not fptr) {
    append("CFitsIO fitsfile pointer is null.");
  } else {
    try {
      append("File name: " + File::name(fptr));
    } catch (...) {
      append("Unknown file name.");
    }
    try {
      append("Current HDU index (0-based): " + std::to_string(Hdu::currentIndex(fptr) - 1));
    } catch (...) {
      append("Unknown current HDU index.");
    }
    try {
      append("Current HDU name: " + Hdu::currentName(fptr));
    } catch (...) {
      append("Unknown current HDU name.");
    }
  }
  append("");
  append(message(status));
  char message[80];
  while (fits_read_errmsg(message) != 0) {
    append(message, 1);
  };
}

std::string CfitsioError::message(int cfitsioStatus) {
  char cfitsioMessage[FLEN_ERRMSG];
  cfitsioMessage[0] = '\0';
  fits_get_errstatus(cfitsioStatus, cfitsioMessage);
  std::string message = "CFitsIO error " + std::to_string(cfitsioStatus) + ": " + cfitsioMessage;
  return message;
}

void CfitsioError::mayThrow(int cfitsioStatus) {
  if (cfitsioStatus != 0) {
    throw CfitsioError(cfitsioStatus);
  }
}

void CfitsioError::mayThrow(int cfitsioStatus, fitsfile* fptr, const std::string& message) {
  if (cfitsioStatus != 0) {
    throw CfitsioError(cfitsioStatus, fptr, message);
  }
}

void mayThrowReadonlyError(fitsfile* fptr) {
  if (not File::isWritable(fptr)) {
    throw CfitsioError(READONLY_FILE);
  }
}

void mayThrowInvalidFileError(fitsfile* fptr) {
  if (not fptr) {
    throw CfitsioError(BAD_FILEPTR);
  }
}

} // namespace Cfitsio
} // namespace Euclid
