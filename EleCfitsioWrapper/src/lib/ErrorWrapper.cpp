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

#include "EleCfitsioWrapper/ErrorWrapper.h"

#include "EleCfitsioWrapper/FileWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Cfitsio {

CfitsioError::CfitsioError(int cfitsioStatus) : FitsError(message(cfitsioStatus)), status(cfitsioStatus) {}

CfitsioError::CfitsioError(int cfitsioStatus, fitsfile* fptr, const std::string& context) :
    FitsError(context), status(cfitsioStatus) {
  append("");
  if (not fptr) {
    append("CFitsIO fitsfile pointer is null.");
  } else {
    try {
      append("File name: " + FileAccess::name(fptr));
    } catch (...) {
      append("Unknown file name.");
    }
    try {
      append("Current HDU index (0-based): " + std::to_string(HduAccess::currentIndex(fptr) - 1));
    } catch (...) {
      append("Unknown current HDU index.");
    }
    try {
      append("Current HDU name: " + HduAccess::currentName(fptr));
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
  float cfitsioVersion = 0;
  fits_get_version(&cfitsioVersion);
  char cfitsioMessage[FLEN_ERRMSG];
  cfitsioMessage[0] = '\0';
  fits_get_errstatus(cfitsioStatus, cfitsioMessage);
  std::string message =
      "CFitsIO v" + std::to_string(cfitsioVersion) + " error " + std::to_string(cfitsioStatus) + ": " + cfitsioMessage;
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
  if (not FileAccess::isWritable(fptr)) {
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
