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
 * @details
 * Provide HDU access/create services.
 * Single Image Fits files can be handled by this class, but SifFile is better suited:
 * it is safer and provide shortcuts.
 * @warning
 * HDU access is provided through references.
 * Reaccessing a given HDU makes any previous reference obsolete:
 * \code
 * const auto &first = f.accessPrimary<>();
 * const auto &second = f.accessPrimary<>();
 * // first is not valid anymore!
 * \endcode
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
   * @brief Get the number of HDUs.
   * @details
   * As opposed to CFitsIO HDU counting, the number is not read by this function:
   * it is initialized by the constructor and then updated at each modification through MefFile methods.
   * This way, incomplete HDUs are also taken into account where CFitsIO would exclude them.
   * This means, for example, that the initial number of HDUs in a new file is 1 instead of 0.
   */
  long hduCount() const;

  /**
   * @brief Read the name of each HDU.
   * @details
   * Unnamed HDUs are taken into account: an empty string is returned for them.
   * @warning
   * readHduNames()[i] is the name of HDU i+1 because Fits index is 1-based.
   */
  std::vector<std::string> readHduNames();

  /**
   * @brief Access the HDU at given index.
   * @tparam T The type of HDU: ImageHdu, BintableHdu, or RecordHdu to just handle metadata.
   * @return A reference to the HDU reader-writer.
   * @details
   * The type can be ImageHdu, BintableHdu or unspecified (i.e. base class RecordHdu, the metadata reader-writer.).
   * In the latter case, if needs be, the returned HDU can still be cast to an ImageHdu or BintableHdu:
   * \code
   * const auto &ext = f.access<>(1);
   * dynamic_cast<ImageHdu&>(ext).readRaster...
   * \endcode
   */
  template <class T = RecordHdu>
  const T &access(long index);

  /**
   * @brief Access the first HDU with given name.
   * @details
   * In the case where several HDUs have the same name, method readHduNames can be used to get the indices.
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
   * @brief Append a new RecordHdu (as an empty ImageHdu) with given name.
   * @return A reference to the new RecordHdu.
   */
  const RecordHdu &initRecordExt(const std::string &name);
  /**
   * @brief Append a new ImageHdu with given name and shape.
   * @details
   * To not only initialize the HDU but also write data, use assignImageExt instead.
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
   * @details
   * To not only initialize the HDU but also write data, use assignBintableExt instead.
   */
  template <typename... Ts>
  const BintableHdu &initBintableExt(const std::string &name, const ColumnInfo<Ts> &... header);

  /**
   * @brief Append a BintableHdu with given name and data.
   * @return A reference to the new BintableHdu.
   * @warning
   * All columns should have the same number of rows.
   */
  template <typename... Ts>
  const BintableHdu &assignBintableExt(const std::string &name, const Column<Ts> &... columns);

protected:
  /**
   * @brief Append an extension.
   * @return A reference to the new HDU of type T.
   */
  template <class T = RecordHdu>
  const T &appendExt(T extension);

  /**
   * @brief Vector of RecordHdus (castable to ImageHdu or BintableHdu).
   * @warning
   * m_hdus is 0-based while Cfitsio HDUs are 1-based.
   */
  std::vector<std::unique_ptr<RecordHdu>> m_hdus;
};

} // namespace FitsIO
} // namespace Euclid

#include "impl/MefFile.hpp"

#endif
