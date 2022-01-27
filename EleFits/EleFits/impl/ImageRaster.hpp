/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#if defined(_ELEFITS_IMAGERASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/ImageRaster.h"

namespace Euclid {
namespace Fits {

template <long N>
Position<N> ImageRaster::readShape() const {
  m_edit();
  return Cfitsio::ImageIo::readShape<N>(m_fptr);
}

template <long N>
void ImageRaster::updateShape(const Position<N>& shape) const {
  m_edit();
  Cfitsio::ImageIo::updateShape<N>(m_fptr, shape);
}

template <typename T, long N>
void ImageRaster::reinit(const Position<N>& shape) const {
  m_edit();
  Cfitsio::ImageIo::updateTypeShape<T, N>(m_fptr, shape);
}

template <typename T, long N>
VecRaster<T, N> ImageRaster::read() const {
  VecRaster<T, N> raster(readShape<N>());
  readTo(raster);
  return raster;
}

template <typename TRaster>
void ImageRaster::readTo(TRaster& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRasterTo(m_fptr, raster);
}

template <typename T, long N, typename TContainer>
void ImageRaster::readTo(Subraster<T, N, TContainer>& subraster) const {
  m_touch();
  Cfitsio::ImageIo::readRasterTo(m_fptr, subraster);
}

template <typename T, long M, long N>
VecRaster<T, M> ImageRaster::readRegion(const Region<N>& region) const {
  VecRaster<T, M> raster(region.shape().template slice<M>());
  readRegionTo(region.front, raster);
  return raster;
}

template <typename TRaster>
void ImageRaster::readRegionTo(FileMemRegions<TRaster::Dim> regions, TRaster& raster) const {
  const auto& memRegion = regions.inMemory();
  if (raster.isContiguous(memRegion)) {
    readRegionToSlice(regions.inFile().front, raster.slice(memRegion));
  } else {
    readRegionToSubraster(regions.inFile().front, raster.subraster(memRegion));
  }
}

template <typename T, long N, typename TContainer>
void ImageRaster::readRegionTo(Subraster<T, N, TContainer>& subraster) const {
  readRegionToSubraster(subraster.region().front, subraster);
}

template <typename TRaster, long N>
void ImageRaster::readRegionToSlice(const Position<N>& frontPosition, TRaster& raster) const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<N>::fromShape(frontPosition, raster.shape()), // FIXME use frontPosition in ImageIo
      raster);
}

template <typename T, long M, long N, typename TContainer>
void ImageRaster::readRegionToSubraster(const Position<N>& frontPosition, Subraster<T, M, TContainer>& subraster)
    const {
  m_touch();
  Cfitsio::ImageIo::readRegionTo(
      m_fptr,
      Region<N>::fromShape(frontPosition, subraster.shape()), // FIXME use frontPosition in ImageIo
      subraster);
  // FIXME move algo here, rely solely on readRegionTo(fitsfile, Position, Raster)
}

template <typename TRaster>
void ImageRaster::write(const TRaster& raster) const {
  m_edit();
  Cfitsio::ImageIo::writeRaster(m_fptr, raster);
}

template <typename TRaster, long N>
void ImageRaster::writeRegion(FileMemRegions<N> regions, const TRaster& raster) const {
  regions.resolve(readShape<N>() - 1, raster.shape() - 1);
  if (raster.isContiguous(regions.memory())) {
    writeSlice(regions.file().front, raster.slice(regions.memory()));
  } else {
    writeSubraster(regions.file().front, raster.subraster(regions.memory()));
  }
}

template <typename T, long N, typename TContainer>
void ImageRaster::writeRegion(const Subraster<T, N, TContainer>& subraster) const {
  writeRegion(subraster.region().front, subraster);
}

template <typename TRaster, long N>
void ImageRaster::writeSlice(const Position<N>& frontPosition, const TRaster& raster) const {
  Cfitsio::ImageIo::writeRegion(m_fptr, raster, frontPosition);
}

template <typename T, long M, long N, typename TContainer>
void ImageRaster::writeSubraster(const Position<N>& frontPosition, const Subraster<T, M, TContainer>& subraster) const {
  m_edit();
  int status = 0;
  auto locus = Region<M>::fromShape(Position<M>::zero(), subraster.shape());
  locus.back[0] = locus.front[0];
  const auto nelem = subraster.shape()[0];
  const auto delta = frontPosition.template slice<M>();
  Position<N> target;
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
