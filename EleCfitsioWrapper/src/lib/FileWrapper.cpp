// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/FileWrapper.h"

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace FileAccess {

fitsfile* create_open(const std::string& filename, CreatePolicy policy) {
  std::string cfitsioName = filename;
  if (policy == CreatePolicy::OverWrite) {
    cfitsioName.insert(0, 1, '!'); // CFITSIO convention
  }
  fitsfile* fptr;
  int status = 0;
  fits_create_file(&fptr, cfitsioName.c_str(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot create file: " + filename);
  HduAccess::init_primary(fptr);
  return fptr;
}

fitsfile* open(const std::string& filename, OpenPolicy policy) {
  fitsfile* fptr;
  int status = 0;
  int permission = READONLY;
  if (policy == OpenPolicy::ReadWrite) {
    permission = READWRITE;
  }
  fits_open_file(&fptr, filename.c_str(), permission, &status);
  CfitsioError::may_throw(status, fptr, "Cannot open file: " + filename);
  return fptr;
}

void close(fitsfile*& fptr) {
  if (not fptr) {
    return;
  }
  int status = 0;
  fits_close_file(fptr, &status);
  CfitsioError::may_throw(status, fptr, "Cannot close file");
  fptr = nullptr;
}

void close_delete(fitsfile*& fptr) {
  if (not fptr) {
    return;
  }
  may_throw_readonly(fptr);
  int status = 0;
  fits_delete_file(fptr, &status);
  fptr = nullptr;
  CfitsioError::may_throw(status, fptr, "Cannot close and delete file");
}

std::string name(fitsfile* fptr) {
  int status = 0;
  char filename[FLEN_FILENAME];
  fits_file_name(fptr, filename, &status);
  CfitsioError::may_throw(status, fptr, "Filename not found");
  return filename;
}

bool is_writable(fitsfile* fptr) {
  int status = 0;
  int filemode;
  fits_file_mode(fptr, &filemode, &status);
  return filemode == READWRITE;
}

} // namespace FileAccess
} // namespace Cfitsio
} // namespace Euclid
