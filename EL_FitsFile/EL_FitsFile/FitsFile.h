/**
 * @file EL_FitsFile/FitsFile.h
 * @date 07/23/19
 * @author user
 *
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

#include <cfitsio/fitsio.h>
#include <string>

#include "EL_FitsFile/RecordHdu.h"
#include "EL_FitsFile/ImageHdu.h"
#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {

/**
 * @brief Wrapper classes to read and write Fits file contents.
 * @details
 * The classes themeselves do not store any data;
 * They just provide IO services, e.g.:
 */
/** @code
FitsFile f("file.fits");
auto raster = f.image_hdu("IMAGE").read_raster<float>();
auto column = f.bintable_hdu("BINTABLE").read_column<std::string>("COL");
@endcode */
/**
 * instantiates an image raster and a bintable column which are neither owned nor referenced by f.
 */
namespace FitsIO {

/**
 * @brief Fits file reader-writer.
 */
class FitsFile {

public:

  /**
   * @brief Fits file permission.
   */
  enum class Permission {
    READ, ///< Open as read-only
    EDIT, ///< Open with write permission
    CREATE, ///< Create a new file
    OVERWRITE, ///< Create a new file or overwrite existing file
    TEMPORARY ///< Create a temporary file (removed by destructor)
  };

  /**
   * @brief Create a new FitsFile with given filename and permission.
   */
  FitsFile(std::string filename, Permission permission);

  /**
   * @brief Destroy the FitsFile and close the file.
   */
  virtual ~FitsFile();

  /**
   * @brief Open a Fits file with given filename and permission.
   */
  void open(std::string filename, Permission permission);

  /**
   * @brief Close the file.
   */
  void close();

  /**
   * @brief Close and delete the file.
   */
  void close_and_delete();

protected:

  fitsfile* m_fptr;
  Permission m_permission;

};

}
}

#endif
