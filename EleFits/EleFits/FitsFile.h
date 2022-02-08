// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_FITSFILE_H
#define _ELEFITS_FITSFILE_H

#include "EleFitsData/FitsError.h"

#include <fitsio.h>
#include <string>

namespace Euclid {

/**
 * @brief Wrapper classes to read and write FITS file contents.
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
 * * `read` and `parse` means that some data are read from the FITS file;
 * * `parse` differs from `read` in that the raw contents of the FITS file are interpreted
 * instead of being simply forwarded:
 * for example, `Header::parse()` returns a `Record` with user-defined type
 * by parsing the characters in the FITS file,
 * while `Hdu::readName()` returns the raw value of the `EXTNAME` keyword.
 * * `write`, `init` and `assign` means that some data are written to the FITS file;
 * * `init` methods write metadata (e.g. image size) while `assign` methods also write data (e.g. image pixels);
 * * Getters -- which do not imply reading from the Files but only working with class members -- are nouns:
 * for example, `Hdu::readName()` is a reading operation, while `Hdu::index()` is a simple getter.
 */
namespace Fits {

/**
 * @brief Version number of the EleFits project.
 */
std::string version();

/**
 * @brief FITS file read/write permissions.
 */
enum class FileMode
{
  Read, ///< Open an existing file with read-only permission
  Edit, ///< Open an existing file with write permission
  Create, ///< Create a new file (overwrite forbidden)
  Write, ///< Open a file if it exists, create a new one otherwise
  Overwrite, ///< Create a new file or overwrite existing file
  Temporary ///< Create a temporary file (removed by destructor, overwrite forbidden)
};

/**
 * @ingroup exceptions
 * @brief Exception thrown if trying to write a read-only file.
 */
class ReadOnlyError : public FitsError {
public:
  /**
   * @brief Constructor.
   * @details
   * The error message is of the form "<prefix>: Tring to write a read-only file (<mode>)".
   */
  explicit ReadOnlyError(const std::string& prefix);

  /**
   * @brief Throw if mode is read-only.
   */
  static void mayThrow(const std::string& prefix, FileMode mode);
};

/**
 * @ingroup file_handlers
 * @brief FITS file reader-writer.
 * @details
 * Mostly en empty shell for file opening and closing operations;
 * Useful services are in the SifFile and MefFile classes.
 * @see \ref handlers
 */
class FitsFile {

public:
  /**
   * @brief Create a new FITS file handler with given filename and permission.
   */
  FitsFile(const std::string& filename, FileMode permission);

  /**
   * @brief Destroy the object and close the file.
   * @details
   * Also remove the file for `FileMode::Temporary`.
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
   * @brief Reopen the file.
   * @details
   * Specific behaviors apply to the following file modes:
   * - `FileMode::Create` and `FileMode::Overwrite`: The file is reopened with `FileMode::Edit`;
   * - `FileMode::Temporary`: The file cannot be reopened.
   */
  void reopen();

  /**
   * @brief Close the file.
   * @details
   * Files opened with `FileMode::Temporary` are deleted after closing by this method.
   */
  void close();

  /**
   * @brief Close and delete the file.
   */
  void closeAndDelete();

  /**
   * @brief Get CFITSIO's `fitsfile*`.
   * @warning
   * There is no way back!
   * @warning
   * The only methods which can safely be called after this one are:
   * - The destructor;
   * - `filename()`.
   * @warning
   * The behaviour of all other handler methods (not only `MefFile`'s ones)
   * after calling this function is undefined.
   */
  fitsfile* handoverToCfitsio();

protected:
  /**
   * @brief Open a FITS file with given filename and permission.
   * @details
   * This method can only be used after having closed the file;
   * It throws an exception otherwise.
   * This method can be used to change the permission:
   * \code
   * FitsFile f(filename, FileMode::Create);
   * ... // Write things
   * f.close();
   * f.open(filename, FileMode::Read);
   * ... // Read things
   * \endcode
   * ... but not to change the filename:
   * If called with another filename, another FITS file will be opened.
   * @warning
   * In any case, relying on the constructors and destructors by managing the object lifetime is preferable.
   */
  void open(const std::string& filename, FileMode permission);

  /**
   * @brief The CFITSIO file handler.
   */
  fitsfile* m_fptr;

  /**
   * @brief The file name.
   */
  std::string m_filename;

  /**
   * @brief The file permission.
   */
  FileMode m_permission;

  /**
   * @brief An open flag to nullify m_fptr at close.
   */
  bool m_open;
};

/**
 * @relates FitsFile
 * @brief Check whether a file exists.
 */
bool fileExists(const std::string& filename);

} // namespace Fits
} // namespace Euclid

#endif
