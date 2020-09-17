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

#include "EL_FitsFile/ImageHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T, long n>
void ImageHdu::resize(const Position<n> &shape) const {
  gotoThisHdu();
  Cfitsio::Image::resize<T, n>(m_fptr, shape);
}

template <typename T, long n>
VecRaster<T, n> ImageHdu::readRaster() const {
  gotoThisHdu();
  return Cfitsio::Image::readRaster<T, n>(m_fptr);
}

template <typename T, long n>
void ImageHdu::writeRaster(const Raster<T, n> &raster) const {
  gotoThisHdu();
  Cfitsio::Image::writeRaster(m_fptr, raster);
}

#ifndef DECLARE_READ_RASTER
#define DECLARE_READ_RASTER(T, n) extern template VecRaster<T, n> ImageHdu::readRaster() const;
DECLARE_READ_RASTER(char, 2)
DECLARE_READ_RASTER(int, 2)
DECLARE_READ_RASTER(float, 2)
DECLARE_READ_RASTER(double, 2)
DECLARE_READ_RASTER(char, 3)
DECLARE_READ_RASTER(int, 3)
DECLARE_READ_RASTER(float, 3)
DECLARE_READ_RASTER(double, 3)
#undef DECLARE_READ_RASTER
#endif

#ifndef DECLARE_WRITE_RASTER
#define DECLARE_WRITE_RASTER(T, n) extern template void ImageHdu::writeRaster(const Raster<T, n> &) const;
DECLARE_WRITE_RASTER(char, 2)
DECLARE_WRITE_RASTER(int, 2)
DECLARE_WRITE_RASTER(float, 2)
DECLARE_WRITE_RASTER(double, 2)
DECLARE_WRITE_RASTER(char, 3)
DECLARE_WRITE_RASTER(int, 3)
DECLARE_WRITE_RASTER(float, 3)
DECLARE_WRITE_RASTER(double, 3)
#undef DECLARE_WRITE_RASTER
#endif

} // namespace FitsIO
} // namespace Euclid
