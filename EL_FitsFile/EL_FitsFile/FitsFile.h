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
 *
 * The following naming conventions are adopted:
 * * `Hdu` refers to both the Primary HDU and the extensions, while `Ext` refers only to the extensions;
 * * `read` and `parse` means that some data are read from the Fits file;
 * * `parse` differs from `read` in that the raw contents of the Fits file are interpretted
 * instead of being simply forwarded:
 * for example, RecordHdu::parseRecord returns a Record with user-defined type
 * by parsing the characters in the Fits file,
 * while RecordHdu::readName returns the raw value of the EXTNAME keyword.
 * * `write`, `update`, `init` and `assign` means that some data are written to the Fits file;
 * * `update` differs from `write` (e.g. RecordHdu::updateRecord vs. RecordHdu::writeRecord)
 * in that if the data already exist in the Fits file, they are overriden instead of new data to be written.
 * * `init` methods write metadata (e.g. image size) while `assign` methods also write data (e.g. image pixels);
 * * Getters -- which do not imply reading from the Files but only working with class members -- are nouns:
 * for example, RecordHdu::readHduName is a reading operation, while RecordHdu::index is a simple getter.
 */
namespace FitsIO {

/**
 * @brief Version number of the EL_FitsIO project.
 */
std::string version();

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
    Read, ///< Open as read-only
    Edit, ///< Open with write permission
    Create, ///< Create a new file (overwrite forbidden)
    Overwrite, ///< Create a new file or overwrite existing file
    Temporary ///< Create a temporary file (removed by destructor, overwrite forbidden)
  };

  /**
   * @brief Create a new Fits file handler with given filename and permission.
   */
  FitsFile(const std::string &filename, Permission permission);

  /**
   * @brief Destroy the object and close the file.
   * @details
   * Also remove the file if permission is FitsFile::Permission::Temporary.
   */
  virtual ~FitsFile();

  /**
   * @brief Get the file name.
   */
  std::string filename() const;

  /**
   * @brief Check whether the file is open.
   */
  bool isOpen() const;

  /**
   * @brief Open a Fits file with given filename and permission.
   * @details
   * This method can only be used after having closed the file;
   * It throws an exception otherwise.
   * This method can be used to change the permission:
   * \code
   * FitsFile f(filename, FitsFile::Permission::CREATE);
   * ... // Write things
   * f.close();
   * f.open(filename, FitsFile::Permission::READ);
   * ... // Read things
   * \endcode
   * ... but not to change the filename:
   * If called with another filename, another Fits file will be opened.
   * @warning
   * In any case, relying on the constructors and destructors by managing the object lifetime is preferrable.
   */
  void open(const std::string &filename, Permission permission);

  /**
   * @brief Reopen the file.
   */
  void reopen();

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
  fitsfile *m_fptr;
  /** @brief The file name. */
  std::string m_filename;
  /** @brief The file permission. */
  Permission m_permission;
  /** @brief An open flag to nullify m_fptr at close. */
  bool m_open;
};

} // namespace FitsIO
} // namespace Euclid

#endif
