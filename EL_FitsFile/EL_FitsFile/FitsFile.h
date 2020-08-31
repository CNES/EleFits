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

#ifndef _EL_FITSFILE_FITSFILE_H
#define _EL_FITSFILE_FITSFILE_H

#include <fitsio.h>
#include <string>

#include "EL_FitsFile/RecordHdu.h"
#include "EL_FitsFile/ImageHdu.h"
#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {

/**
 * @brief Wrapper classes to read and write Fits file contents.
 * @details
 * There are two types of classes:
 * -# Service classes offer read/write services:
 *   - File handlers (XxxFile classes) only store index and type of HDUs which have already been accessed.
 *   - HDU handlers (XxxHdu classes) only provide read/write services.
 *     When you access an HDU, you just access a set of services to read and write items in this HDU.
 * -# Data classes -- Record, Raster and Column -- store data to be read and written.
 */
namespace FitsIO {

/**
 * @brief Fits file reader-writer.
 * @details
 * Mostly en empty shell for file opening and closing operations;
 * Useful services are in the SifFile and MefFile classes.
 * @see \ref handlers
 */
class FitsFile {

public:

  /**
   * @brief Fits file read/write permissions.
   */
  enum class Permission {
    READ, ///< Open as read-only
    EDIT, ///< Open with write permission
    CREATE, ///< Create a new file (overwrite forbidden)
    OVERWRITE, ///< Create a new file or overwrite existing file
    TEMPORARY ///< Create a temporary file (removed by destructor, overwrite forbidden)
  };

  /**
   * @brief Create a new Fits file handler with given filename and permission.
   */
  FitsFile(const std::string& filename, Permission permission);

  /**
   * @brief Destroy the FitsFile and close the file.
   * @details Also remove the file if permission is Permission::TEMPORARY.
   */
  virtual ~FitsFile();

  /**
   * @brief Get the file name.
   */
  std::string filename() const;

  /**
   * @brief Open a Fits file with given filename and permission.
   */
  void open(const std::string& filename, Permission permission);

  /**
   * @brief Close the file.
   */
  void close();

  /**
   * @brief Close and delete the file.
   */
  void closeAndDelete();

protected:

  /** @brief The CFitsIO file handler. */
  fitsfile* m_fptr;
  /** @brief The file name. */
  std::string m_filename;
  /** @brief The file permission. */
  Permission m_permission;
  /** @brief An open flag to nullify m_fptr at close. */
  bool m_open;

};

}
}

#endif
