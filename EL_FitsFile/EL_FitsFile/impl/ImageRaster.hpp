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

template <typename T, long n>
ImageRaster<T, n>::ImageRaster(
    fitsfile*& fptr,
    std::function<void(void)> touchFunc,
    std::function<void(void)> editFunc) :
    m_fptr(fptr),
    m_touch(touchFunc), m_edit(editFunc) {}

template <typename T, long n>
const std::type_info& ImageRaster<T, n>::readTypeid() const {
  m_touch();
  return Cfitsio::Image::readTypeid(m_fptr);
}

template <typename T, long n>
long ImageRaster<T, n>::readSize() const {
  return shapeSize(readShape());
}

template <typename T, long n>
Position<n> ImageRaster<T, n>::readShape() const {
  m_edit();
  return Cfitsio::Image::readShape<n>(m_fptr);
}

template <typename T, long n>
void ImageRaster<T, n>::updateShape(const Position<n>& shape) const {
  m_edit();
  Cfitsio::Image::updateShape<T, n>(m_fptr, shape);
}

template <typename T, long n>
template <typename U, long m>
ImageRaster<U, m> ImageRaster<T, n>::reInit(const Position<m>& shape) {
  m_edit();
  Cfitsio::Image::updateShape<T, n>(m_fptr, shape);
  return ImageRaster<U, m>(m_fptr, m_touch, m_edit); // TODO how to invalidate *this
}

template <typename T, long n>
VecRaster<T, n> ImageRaster<T, n>::read() const {
  m_touch();
  return Cfitsio::Image::readRaster<T, n>(m_fptr);
}

template <typename T, long n>
void ImageRaster<T, n>::readTo(Raster<T, n>& raster) const {
  m_touch();
  Cfitsio::Image::readRasterTo<T, n>(m_fptr, raster);
}

template <typename T, long n>
void ImageRaster<T, n>::readTo(Subraster<T, n>& subraster) const {
  m_touch();
  Cfitsio::Image::readRasterTo<T, n>(m_fptr, subraster);
}

template <typename T, long n>
VecRaster<T, n> ImageRaster<T, n>::readRegion(const Region<n>& region) const {
  m_touch();
  return Cfitsio::Image::readRegion<T, n>(m_fptr, region);
}

template <typename T, long n>
void ImageRaster<T, n>::readRegionTo(const Region<n>& region, Raster<T, n>& raster) const {
  m_touch();
  Cfitsio::Image::readRegionTo<T, n>(m_fptr, region, raster);
}

template <typename T, long n>
void ImageRaster<T, n>::readRegionTo(const Region<n>& region, Subraster<T, n>& subraster) const {
  m_touch();
  Cfitsio::Image::readRegionTo<T, n>(m_fptr, region, subraster);
}

template <typename T, long n>
void ImageRaster<T, n>::write(const Raster<T, n>& raster) const {
  m_edit();
  Cfitsio::Image::writeRaster<T, n>(m_fptr, raster);
}

template <typename T, long n>
void ImageRaster<T, n>::writeRegion(const Raster<T, n>& raster, const Position<n>& destination) {
  m_edit();
  Cfitsio::Image::writeRegion<T, n>(m_fptr, raster, destination);
}

template <typename T, long n>
void ImageRaster<T, n>::writeRegion(const Subraster<T, n>& subraster) {
  writeRegion(subraster, subraster.region.front);
}

template <typename T, long n>
void ImageRaster<T, n>::writeRegion(const Subraster<T, n>& subraster, const Position<n>& destination) {
  m_edit();
  Cfitsio::Image::writeRegion<T, n>(m_fptr, subraster, destination);
}

} // namespace FitsIO
} // namespace Euclid

#endif
