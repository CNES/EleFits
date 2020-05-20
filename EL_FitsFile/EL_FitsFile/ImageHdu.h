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

#ifndef _EL_FITSFILE_IMAGEHDU_H
#define _EL_FITSFILE_IMAGEHDU_H

#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {


/**
 * @brief Image HDU reader-writer.
 */
class ImageHdu : public RecordHdu {

public:

  /**
   * @brief Constructor.
   * @warning
   * You should not instantiate RecordHdus yourself,
   * but using the dedicated MefFile creation method.
   */
  ImageHdu(fitsfile*& fptr, std::size_t index);

  /**
   * @brief Destructor.
   */
  virtual ~ImageHdu() = default;

  /**
   * @brief Redefine the image shape and type.
   */
  template<typename T, std::size_t n=2>
  void resize(const pos_type<n>& shape) const;

  /**
   * @brief Read the Raster.
   */
  template<typename T, std::size_t n=2>
  VecRaster<T, n> read_raster() const;

  /**
   * @brief Write the Raster.
   */
  template<typename T, std::size_t n=2>
  void write_raster(const Raster<T, n>& raster) const;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, std::size_t n>
void ImageHdu::resize(const pos_type<n>& shape) const {
  goto_this_hdu();
  Cfitsio::Image::resize<T, n>(m_fptr, shape);
}

template<typename T, std::size_t n>
VecRaster<T, n> ImageHdu::read_raster() const {
  goto_this_hdu();
  return Cfitsio::Image::read_raster<T, n>(m_fptr);
}

template<typename T, std::size_t n>
void ImageHdu::write_raster(const Raster<T, n>& raster) const {
  goto_this_hdu();
  Cfitsio::Image::write_raster(m_fptr, raster);
}

#define DECLARE_READ_RASTER(T, n) \
  extern template VecRaster<T, n> ImageHdu::read_raster() const;
DECLARE_READ_RASTER(char, 2)
DECLARE_READ_RASTER(int, 2)
DECLARE_READ_RASTER(float, 2)
DECLARE_READ_RASTER(double, 2)
DECLARE_READ_RASTER(char, 3)
DECLARE_READ_RASTER(int, 3)
DECLARE_READ_RASTER(float, 3)
DECLARE_READ_RASTER(double, 3)

#define DECLARE_WRITE_RASTER(T, n) \
  extern template void ImageHdu::write_raster(const Raster<T, n>&) const;
DECLARE_WRITE_RASTER(char, 2)
DECLARE_WRITE_RASTER(int, 2)
DECLARE_WRITE_RASTER(float, 2)
DECLARE_WRITE_RASTER(double, 2)
DECLARE_WRITE_RASTER(char, 3)
DECLARE_WRITE_RASTER(int, 3)
DECLARE_WRITE_RASTER(float, 3)
DECLARE_WRITE_RASTER(double, 3)

}
}

#endif
