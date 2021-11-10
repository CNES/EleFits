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

#if defined(_ELEFITS_IMAGERASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/ImageRaster.h"

namespace Euclid {
namespace Fits {

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
  VecRaster<T, m> raster(region.shape().template slice<m>());
  readRegionTo(region.front, raster);
  return raster;
}

template <typename T, long m, long n>
void ImageRaster::readRegionTo(FileMemRegions<n> regions, Raster<T, m>& raster) const {
  const auto& memRegion = regions.inMemory();
  if (raster.isContiguous(memRegion)) {
    readRegionToSlice(regions.inFile().front, raster.slice(memRegion));
  } else {
    readRegionToSubraster(regions.inFile().front, raster.subraster(memRegion));
  }
}

template <typename T, long n>
void ImageRaster::readRegionTo(Subraster<T, n>& subraster) const {
  readRegionToSubraster(subraster.region().front, subraster);
}

template <typename T, long m, long n>
void ImageRaster::readRegionToSlice(const Position<n>& frontPosition, Raster<T, m>& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<n>::fromShape(frontPosition, raster.shape()), // FIXME use frontPosition in ImageIo
      raster);
}

template <typename T, long m, long n>
void ImageRaster::readRegionToSubraster(const Position<n>& frontPosition, Subraster<T, m>& subraster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<n>::fromShape(frontPosition, subraster.shape()), // FIXME use frontPosition in ImageIo
      subraster);
  // FIXME move algo here, rely solely on readRegionTo(fitsfile, Position, Raster)
}

template <typename T, long n>
void ImageRaster::write(const Raster<T, n>& raster) const {
  m_edit();
  Cfitsio::ImageIo::writeRaster<T, n>(m_fptr, raster);
}

template <typename T, long m, long n>
void ImageRaster::writeRegion(FileMemRegions<n> regions, const Raster<T, m>& raster) const {
  regions.resolve(readShape<n>() - 1, raster.shape() - 1);
  if (raster.isContiguous(regions.memory())) {
    writeSlice(regions.file().front, raster.slice(regions.memory()));
  } else {
    writeSubraster(regions.file().front, raster.subraster(regions.memory()));
  }
}

template <typename T, long n>
void ImageRaster::writeRegion(const Subraster<T, n>& subraster) const {
  writeRegion(subraster.region().front, subraster);
}

template <typename T, long m, long n>
void ImageRaster::writeSlice(const Position<n>& frontPosition, const Raster<T, m>& raster) const {
  Cfitsio::ImageIo::writeRegion(m_fptr, raster, frontPosition);
}

template <typename T, long m, long n>
void ImageRaster::writeSubraster(const Position<n>& frontPosition, const Subraster<T, m>& subraster) const {
  m_edit();
  int status = 0;
  auto locus = Region<m>::fromShape(Position<m>::zero(), subraster.shape());
  locus.back[0] = locus.front[0];
  const auto nelem = subraster.shape()[0];
  const auto delta = frontPosition.template slice<m>();
  Position<n> target;
  for (const auto& source : locus) {
    target = (source + delta).extend(frontPosition) + 1; // 1-based
    const auto b = &subraster[source];
    const auto e = b + nelem;
    std::vector<std::decay_t<T>> nonconstData(b, e);
    fits_write_pix(m_fptr, Cfitsio::TypeCode<T>::forImage(), target.data(), nelem, nonconstData.data(), &status);
    // TODO to ImageWrapper
  }
}

} // namespace Fits
} // namespace Euclid

#endif
