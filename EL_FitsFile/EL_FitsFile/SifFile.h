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

#ifndef _EL_FITSFILE_SIFFILE_H
#define _EL_FITSFILE_SIFFILE_H

#include "EL_FitsFile/FitsFile.h"
#include "EL_FitsFile/ImageHdu.h"

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup handlers
 * @brief Single image Fits file handler.
 * @details
 * Provide read/write access to the header and image raster.
 * @see \ref handlers
 */
class SifFile : public FitsFile {

public:
  /**
   * @copydoc FitsFile::Permission
   */
  using FitsFile::Permission;

  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~SifFile() = default;

  /**
   * @copydoc FitsFile::FitsFile
   */
  SifFile(const std::string& filename, Permission permission);

  /**
   * @brief Access the header.
   * @warning
   * Return will be of type Header from version 4.0 on.
   */
  const RecordHdu& header() const; // FIXME return const Header&

  /**
   * @brief Read the raster.
   */
  template <typename T, long n = 2>
  VecRaster<T, n> readRaster() const;

  /**
   * @brief Write the raster (initialize primary HDU if not done).
   */
  template <typename T, long n>
  void writeRaster(const Raster<T, n>& raster) const;

private:
  /** @brief The Primary (and only) HDU */
  ImageHdu m_hdu;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_SIFFILE_IMPL
#include "EL_FitsFile/impl/SifFile.hpp"
#undef _EL_FITSFILE_SIFFILE_IMPL
/// @endcond

#endif
