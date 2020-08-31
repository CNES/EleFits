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
 * @brief Single image Fits file handler.
 * @details Provide read/write access to the header and image raster.
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
   */
  const RecordHdu& header() const;

  /**
   * @brief Read the Raster.
   */
  template<typename T, long n=2>
  VecRaster<T, n> read_raster() const;

  /**
   * @brief Write the Raster (initialize primary HDU if not done).
   */
  template<typename T, long n>
  void write_raster(const Raster<T, n>& raster) const;

private:

  ImageHdu m_hdu;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, long n>
VecRaster<T, n> SifFile::read_raster() const {
  return Cfitsio::Image::readRaster<T, n>(m_fptr);
}

template<typename T, long n>
void SifFile::write_raster(const Raster<T, n>& raster) const {
  Cfitsio::Hdu::gotoPrimary(m_fptr);
  Cfitsio::Image::resize<T, n>(m_fptr, raster.shape);
  Cfitsio::Image::writeRaster(m_fptr, raster);
}

}
}

#endif
