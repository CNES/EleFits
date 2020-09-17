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

#ifndef _EL_FITSFILE_MEFFILE_H
#define _EL_FITSFILE_MEFFILE_H

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_FitsFile/FitsFile.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Multi-Extension Fits file reader-writer.
 * @details Provide HDU access/create services.
 * @warning HDU access is provided through references.
 * Reaccessing a given HDU makes any previous reference obsolete.
 * @see \ref handlers
 */
class MefFile : public FitsFile {

public:
  /**
   * @copydoc FitsFile::Permission
   */
  using FitsFile::Permission;

  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~MefFile() = default;

  /**
   * @copydoc FitsFile::FitsFile
   */
  MefFile(const std::string &filename, Permission permission);

  /**
   * @brief Count the number of HDUs.
   */
  long hduCount() const;

  /**
   * @brief Read the name of each HDU.
   * @warning hduNames()[i] is the name of HDU i+1 because Fits index is 1-based.
   */
  std::vector<std::string> hduNames();

  /**
   * @brief Access the Hdu at given index.
   * @tparam The type of Hdu: ImageHdu, BintableHdu or RecordHdu to just handle metadata.
   * @return A reference to the HDU reader-writer.
   * @details
   * The type can be ImageHdu, BintableHdu or unspecified (i.e. base class RecordHdu).
   * In the latter case, if needs be, the returned Hdu can still be cast to an ImageHdu or BintableHdu
   * (e.g., \c dynamic_cast<ImageHdu&>(hdu) )
   * or merely be used as a metadata reader-writer.
   */
  template <class T = RecordHdu>
  const T &access(long index);

  /**
   * @brief Access the first HDU with given name.
   * @see access
   */
  template <class T = RecordHdu>
  const T &accessFirst(const std::string &name);

  /**
   * @brief Access the Primary HDU.
   * @see access
   */
  template <class T = RecordHdu>
  const T &accessPrimary();

  /**
   * @brief Append an extension.
   * @return A reference to the new HDU of type T.
   */
  template <class T = RecordHdu>
  const T &appendExt(T extension);

  /**
   * @brief Append a new RecordHdu (as empty ImageHdu) with given name.
   * @return A reference to the new RecordHdu.
   */
  const RecordHdu &initRecordExt(const std::string &name);
  /**
   * @brief Append a new ImageHdu with given name and shape.
   * @see assignImageExt
   */
  template <typename T, long n>
  const ImageHdu &initImageExt(const std::string &name, const Position<n> &shape);

  /**
   * @brief Append an ImageHdu with given name and data.
   * @return A reference to the new ImageHdu.
   */
  template <typename T, long n>
  const ImageHdu &assignImageExt(const std::string &name, const Raster<T, n> &raster);

  /**
   * @brief Append a BintableHdu with given name and columns info.
   * @see assignBintableExt
   */
  template <typename... Ts>
  const BintableHdu &initBintableExt(const std::string &name, const ColumnInfo<Ts> &... header);

  /**
   * @brief Append a BintableHdu with given name and data.
   * @warning All columns should have the same number of rows.
   * @return A reference to the new BintableHdu.
   */
  template <typename... Ts>
  const BintableHdu &assignBintableExt(const std::string &name, const Column<Ts> &... columns);

protected:
  /**
   * @brief Vector of RecordHdus (castable to ImageHdu or BintableHdu).
   * @warning m_hdus is 0-based while Cfitsio HDUs are 1-based.
   */
  std::vector<std::unique_ptr<RecordHdu>> m_hdus;
};

} // namespace FitsIO
} // namespace Euclid

#include "impl/MefFile.hpp"

#endif
