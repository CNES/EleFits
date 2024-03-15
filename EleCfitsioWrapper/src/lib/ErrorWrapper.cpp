// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/ErrorWrapper.h"

#include "EleCfitsioWrapper/FileWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"

namespace Cfitsio {

CfitsioError::CfitsioError(int cfitsio_status) : FitsError(message(cfitsio_status)), status(cfitsio_status) {}

CfitsioError::CfitsioError(int cfitsio_status, fitsfile* fptr, const std::string& context) :
    FitsError(context), status(cfitsio_status)
{
  append("");
  if (not fptr) {
    append("CFITSIO fitsfile pointer is null.");
  } else {
    try {
      append("File name: " + FileAccess::name(fptr));
    } catch (...) {
      append("Unknown file name.");
    }
    try {
      append("Current HDU index (0-based): " + std::to_string(HduAccess::current_index(fptr) - 1));
    } catch (...) {
      append("Unknown current HDU index.");
    }
    try {
      append("Current HDU name: " + HduAccess::current_name(fptr));
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

std::string CfitsioError::message(int cfitsio_status)
{
  float cfitsio_version = 0;
  fits_get_version(&cfitsio_version);
  char cfitsio_message[FLEN_ERRMSG];
  cfitsio_message[0] = '\0';
  fits_get_errstatus(cfitsio_status, cfitsio_message);
  std::string message = "CFITSIO v" + std::to_string(cfitsio_version) + " error " + std::to_string(cfitsio_status) +
      ": " + cfitsio_message;
  return message;
}

void CfitsioError::may_throw(int cfitsio_status)
{
  if (cfitsio_status != 0) {
    throw CfitsioError(cfitsio_status);
  }
}

void CfitsioError::may_throw(int cfitsio_status, fitsfile* fptr, const std::string& message)
{
  if (cfitsio_status != 0) {
    throw CfitsioError(cfitsio_status, fptr, message);
  }
}

void may_throw_readonly(fitsfile* fptr)
{
  if (not FileAccess::is_writable(fptr)) {
    throw CfitsioError(READONLY_FILE);
  }
}

void may_throw_invalid_file(fitsfile* fptr)
{
  if (not fptr) {
    throw CfitsioError(BAD_FILEPTR);
  }
}

} // namespace Cfitsio
