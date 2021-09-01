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

#if defined(_EL_FITSFILE_IMAGERASTER_IMPL) || defined(CHECK_QUALITY)

  #include "EL_CfitsioWrapper/ImageWrapper.h"
  #include "EL_FitsFile/ImageRaster.h"

namespace Euclid {
namespace FitsIO {

template <long n>
Position<n> ImageRaster::readShape() const {
  m_edit();
  return Cfitsio::ImageIo::readShape<n>(m_fptr);
}

template <long n>
void ImageRaster::updateShape(const Position<n>& shape) const {
  m_edit();
  Cfitsio::ImageIo::updateShape<n>(m_fptr, shape);
}

template <typename T, long n>
void ImageRaster::reinit(const Position<n>& shape) const {
  m_edit();
  Cfitsio::ImageIo::updateTypeShape<T, n>(m_fptr, shape);
}

template <typename T, long n>
VecRaster<T, n> ImageRaster::read() const {
  m_touch();
  return Cfitsio::ImageIo::readRaster<T, n>(m_fptr);
}

template <typename T, long n>
void ImageRaster::readTo(Raster<T, n>& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRasterTo<T, n>(m_fptr, raster);
}

template <typename T, long n>
void ImageRaster::readTo(Subraster<T, n>& subraster) const {
  m_touch();
  Cfitsio::ImageIo::readRasterTo<T, n>(m_fptr, subraster);
}

template <typename T, long n>
VecRaster<T, n> ImageRaster::readRegion(const Region<n>& region) const {
  m_touch();
  return Cfitsio::ImageIo::readRegion<T, n>(m_fptr, region);
}

template <typename T, long n>
void ImageRaster::readRegionTo(const Region<n>& region, Raster<T, n>& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo<T, n>(m_fptr, region, raster);
}

template <typename T, long n>
void ImageRaster::readRegionTo(const Region<n>& region, Subraster<T, n>& subraster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo<T, n>(m_fptr, region, subraster);
}

template <typename T, long n>
void ImageRaster::write(const Raster<T, n>& raster) const {
  m_edit();
  Cfitsio::ImageIo::writeRaster<T, n>(m_fptr, raster);
}

template <typename T, long n>
void ImageRaster::writeRegion(const Raster<T, n>& raster, const Position<n>& destination) {
  m_edit();
  Cfitsio::ImageIo::writeRegion<T, n>(m_fptr, raster, destination);
}

template <typename T, long n>
void ImageRaster::writeRegion(const Subraster<T, n>& subraster) {
  writeRegion(subraster, subraster.region.front);
}

template <typename T, long n>
void ImageRaster::writeRegion(const Subraster<T, n>& subraster, const Position<n>& destination) {
  m_edit();
  Cfitsio::ImageIo::writeRegion<T, n>(m_fptr, subraster, destination);
}

} // namespace FitsIO
} // namespace Euclid

#endif
