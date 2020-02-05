/**
 * @file EL_FitsFile/SifFile.h
 * @date 08/30/19
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

#ifndef _EL_FITSFILE_SIFFILE_H
#define _EL_FITSFILE_SIFFILE_H

#include "EL_FitsFile/FitsFile.h"
#include "EL_FitsFile/ImageHdu.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Single image Fits file handler.
 * @details Provide read/write access to the header and image raster.
 */
class SifFile : public FitsFile {

public:

  /**
   * @see FitsFile::Permission
   */
  using FitsFile::Permission;

  /**
   * @see FitsFile::~FitsFile
   */
  virtual ~SifFile() = default;

  /**
   * @see FitsFile::FitsFile
   */
  SifFile(std::string filename, Permission permission);

	/**
	 * @brief Access the header.
	 */
  const RecordHdu& header() const;

  /**
   * @brief Read the Raster.
   */
  template<typename T, std::size_t n=2>
  VecRaster<T, n> read_raster() const;

  /**
   * @brief Write the Raster (initialize primary HDU if not done).
   */
  template<typename T, std::size_t n>
  void write_raster(const Raster<T, n>& raster) const;

private:

  ImageHdu m_hdu;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
VecRaster<T, n> SifFile::read_raster() const {
	return Cfitsio::Image::read_raster<T, n>(m_fptr);
}

template<typename T, std::size_t n>
void SifFile::write_raster(const Raster<T, n>& raster) const {
  Cfitsio::Hdu::goto_primary(m_fptr); //TODO useful?
  Cfitsio::Image::resize<T, n>(m_fptr, raster.shape);
  Cfitsio::Image::write_raster(m_fptr, raster);
}

}
}

#endif
