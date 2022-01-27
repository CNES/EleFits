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

#if defined(_ELEFITS_IMAGEHDU_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/ImageHdu.h"

namespace Euclid {
namespace Fits {

template <long n>
Position<n> ImageHdu::readShape() const {
  return m_raster.readShape<n>();
}

template <typename T, long n>
void ImageHdu::updateShape(const Position<n>& shape) const {
  return m_raster.reinit<T, n>(shape);
}

template <typename T, long n>
VecRaster<T, n> ImageHdu::readRaster() const {
  return m_raster.read<T, n>();
}

template <typename TRaster>
void ImageHdu::writeRaster(const TRaster& data) const {
  m_raster.write(data);
}

/**
 * @brief Specialization of `Hdu::as()` for the data unit.
 */
template <>
const ImageRaster& Hdu::as() const;

#ifndef DECLARE_READ_RASTER
#define DECLARE_READ_RASTER(type, unused) \
  extern template VecRaster<type, -1> ImageHdu::readRaster() const; \
  extern template VecRaster<type, 2> ImageHdu::readRaster() const; \
  extern template VecRaster<type, 3> ImageHdu::readRaster() const;
ELEFITS_FOREACH_RASTER_TYPE(DECLARE_READ_RASTER)
#undef DECLARE_READ_RASTER
#endif

#ifndef DECLARE_WRITE_RASTER
#define DECLARE_WRITE_RASTER(type, unused) \
  extern template void ImageHdu::writeRaster(const PtrRaster<type, -1>&) const; \
  extern template void ImageHdu::writeRaster(const PtrRaster<type, 2>&) const; \
  extern template void ImageHdu::writeRaster(const PtrRaster<type, 3>&) const; \
  extern template void ImageHdu::writeRaster(const VecRaster<type, -1>&) const; \
  extern template void ImageHdu::writeRaster(const VecRaster<type, 2>&) const; \
  extern template void ImageHdu::writeRaster(const VecRaster<type, 3>&) const;
ELEFITS_FOREACH_RASTER_TYPE(DECLARE_WRITE_RASTER)
#undef DECLARE_WRITE_RASTER
#endif

} // namespace Fits
} // namespace Euclid

#endif
