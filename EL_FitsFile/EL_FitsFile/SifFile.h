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
 */
class SifFile : public FitsFile {

public:

  using FitsFile::Permission;

  virtual ~SifFile() = default;

  SifFile(std::string filename, Permission permission);

  template<typename T, std::size_t n>
  SifFile(std::string filename, Permission permission, const Raster<T, n>& raster);

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
   * @brief Write the Raster (initialize HDU if not done).
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
SifFile::SifFile(std::string filename, Permission permission, const Raster<T, n>& raster) :
    FitsFile(filename, permission),
    m_hdu("", 1) {
  write_raster(raster);
}

template<typename T, std::size_t n>
VecRaster<T, n> SifFile::read_raster() const {
	return Cfitsio::Image::read_raster<T, n>(m_fptr);
}

template<typename T, std::size_t n>
void SifFile::write_raster(const Raster<T, n>& raster) const {
  if(Cfitsio::Hdu::count(m_fptr) == 0)
    throw std::runtime_error("Primary not initialized.");
  Cfitsio::Hdu::goto_primary(m_fptr); //TODO useful?
  Cfitsio::Image::write_raster(m_fptr, raster);
}

}
}

#endif
