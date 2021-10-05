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
  VecRaster<T, n> raster(readShape<n>());
  readTo<T, n>(raster);
  return raster;
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

template <typename T, long m, long n>
VecRaster<T, m> ImageRaster::readRegion(const Region<n>& region) const {
  Position<m> shape(m);
  for (long i = 0; i < m; ++i) {
    shape[i] = region.shape()[i]; // FIXME as Position::slice<m>();
  }
  VecRaster<T, m> raster(shape); // FIXME region.shape().slice<m>());
  readRegionTo(region.front, raster);
  return raster;
}

template <typename T, long m, long n>
void ImageRaster::readRegionTo(FileMemRegions<n> regions, Raster<T, m>& raster) const {
  const auto& memRegion = regions.inMemory();
  if (raster.isContiguous(memRegion)) {
    readRegionTo(regions.inFile().front, raster.slice(memRegion));
  } else {
    readRegionTo(regions.inFile().front, raster.subraster(memRegion));
  }
}

template <typename T, long m, long n>
void ImageRaster::readRegionTo(const Position<n>& frontPosition, Raster<T, m>& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<n>::fromShape(frontPosition, raster.shape),
      raster); // FIXME use frontPosition in ImageIo
}

template <typename T, long m, long n>
void ImageRaster::readRegionTo(const Position<n>& frontPosition, Subraster<T, m>& subraster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<n>::fromShape(frontPosition, subraster.shape()),
      subraster); // FIXME use frontPosition in ImageIo
  // FIXME move algo here, rely solely on readRegionTo(fitsfile, Position, Raster)
}

template <typename T, long n>
void ImageRaster::write(const Raster<T, n>& raster) const {
  m_edit();
  Cfitsio::ImageIo::writeRaster<T, n>(m_fptr, raster);
}

template <typename T, long m, long n>
void ImageRaster::writeRegion(FileMemRegions<n> regions, const Raster<T, m>& raster) const {
  const bool memBackWasMax = regions.setMemoryBackIfMax(raster.domain().back);
  if (not memBackWasMax && regions.file().back.isMax()) {
    const auto fileBack = regions.file().front + readShape<n>() - 1;
    regions.setFileBackIfMax(fileBack);
  }
  if (raster.isContiguous(regions.memory())) {
    writeRegion(regions.file().front, raster.slice(regions.memory()));
  } else {
    writeRegion(regions.file().front, raster.subraster(regions.memory()));
  }
}

template <typename T, long m, long n>
void ImageRaster::writeRegion(const Position<n>& frontPosition, const Raster<T, m>& raster) const {
  m_edit();
  Cfitsio::ImageIo::writeRegion(m_fptr, raster, frontPosition);
}

template <typename T, long n>
void ImageRaster::writeRegion(const Subraster<T, n>& subraster) const {
  writeRegion(subraster.region().front, subraster);
}

template <typename T, long m, long n>
void ImageRaster::writeRegion(const Position<n>& frontPosition, const Subraster<T, m>& subraster) const {
  m_edit();
  Cfitsio::ImageIo::writeRegion(m_fptr, subraster, frontPosition);
  // FIXME move algo here, rely solely on writeRegion(fitsfile, Raster, Position)
}

} // namespace FitsIO
} // namespace Euclid

#endif
