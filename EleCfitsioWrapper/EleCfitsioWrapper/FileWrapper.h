// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_FILEWRAPPER_H
#define _ELECFITSIOWRAPPER_FILEWRAPPER_H

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief File-related functions.
 */
namespace FileAccess {

/**
 * @brief File creation policy.
 */
enum class CreatePolicy {
  CreateOnly, ///< Create only (abort if file exists).
  OverWrite ///< Allow overwriting if file already exists.
};

/**
 * @brief File opening policy.
 */
enum class OpenPolicy {
  ReadOnly, ///< Read persmission only.
  ReadWrite ///< Read and write permissions.
};

/**
 * @brief Create or overwrite a FITS file and open it.
 */
fitsfile* create_open(const std::string& filename, CreatePolicy policy);

/**
 * @brief Open an existing FITS file with optional write permission.
 */
fitsfile* open(const std::string& filename, OpenPolicy policy);

/**
 * @brief Close a FITS file.
 */
void close(fitsfile*& fptr);

/**
 * @brief Close and delete a FITS file.
 * @warning Throw an exception if writing is not permitted.
 */
void close_delete(fitsfile*& fptr);

/**
 * @brief Get the file name.
 */
std::string name(fitsfile* fptr);

/**
 * @brief Check whether a FITS file is open with write permission.
 */
bool is_writable(fitsfile* fptr);

} // namespace FileAccess
} // namespace Cfitsio
} // namespace Euclid

#endif
