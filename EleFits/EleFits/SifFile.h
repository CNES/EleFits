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

#ifndef _ELEFITS_SIFFILE_H
#define _ELEFITS_SIFFILE_H

#include "EleFits/FitsFile.h"
#include "EleFits/ImageHdu.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup file_handlers
 * @brief Single image Fits file handler.
 * @details
 * Provide read/write access to the header and image raster.
 * @see \ref handlers
 */
class SifFile : public FitsFile {

public:
  /**
   * @copydoc FitsFile::~FitsFile
   */
  virtual ~SifFile() = default;

  /**
   * @copydoc FitsFile::FitsFile
   */
  SifFile(const std::string& filename, FileMode permission);

  /**
   * @brief Access the header unit.
   * @warning
   * Return will be of type Header from version 4.0 on.
   */
  const Header& header() const;

  /**
   * @brief Access the data unit.
   */
  const ImageRaster& raster() const;

  /**
   * @brief Write both the records and the raster.
   */
  template <typename T, long n = 2>
  void writeAll(const RecordSeq& records, const Raster<T, n>& raster);

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

  /**
   * @copydoc Hdu::verifyChecksums()
   */
  void verifyChecksums() const;

  /**
   * @copydoc Hdu::updateChecksums()
   */
  void updateChecksums() const;

private:
  /**
   * @brief The Primary (and only) HDU
   */
  ImageHdu m_hdu;

  /**
   * @brief The header unit.
   */
  const Header& m_header;

  /**
   * @brief The data unit.
   */
  const ImageRaster& m_raster;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_SIFFILE_IMPL
#include "EleFits/impl/SifFile.hpp"
#undef _ELEFITS_SIFFILE_IMPL
/// @endcond

#endif
