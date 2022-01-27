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
enum class CreatePolicy
{
  CreateOnly, ///< Create only (abort if file exists).
  OverWrite ///< Allow overwriting if file already exists.
};

/**
 * @brief File opening policy.
 */
enum class OpenPolicy
{
  ReadOnly, ///< Read persmission only.
  ReadWrite ///< Read and write permissions.
};

/**
 * @brief Create or overwrite a Fits file and open it.
 */
fitsfile* createAndOpen(const std::string& filename, CreatePolicy policy);

/**
 * @brief Open an existing Fits file with optional write permission.
 */
fitsfile* open(const std::string& filename, OpenPolicy policy);

/**
 * @brief Close a Fits file.
 */
void close(fitsfile*& fptr);

/**
 * @brief Close and delete a Fits file.
 * @warning Throw an exception if writing is not permitted.
 */
void closeAndDelete(fitsfile*& fptr);

/**
 * @brief Get the file name.
 */
std::string name(fitsfile* fptr);

/**
 * @brief Check whether a Fits file is open with write permission.
 */
bool isWritable(fitsfile* fptr);

} // namespace FileAccess
} // namespace Cfitsio
} // namespace Euclid

#endif
