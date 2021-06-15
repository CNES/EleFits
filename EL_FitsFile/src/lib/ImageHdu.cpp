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

ImageHdu::ImageHdu(Token token, fitsfile*& fptr, long index) : RecordHdu(token, fptr, index, HduCategory::Image) {}

const std::type_info& ImageHdu::readTypeid() const {
  gotoThisHdu();
  return Cfitsio::Image::readTypeid(m_fptr);
}

#ifndef COMPILE_READ_RASTER
  #define COMPILE_READ_RASTER(type, unused) \
    template VecRaster<type, -1> ImageHdu::readRaster() const; \
    template VecRaster<type, 2> ImageHdu::readRaster() const; \
    template VecRaster<type, 3> ImageHdu::readRaster() const;
EL_FITSIO_FOREACH_RASTER_TYPE(COMPILE_READ_RASTER)
  #undef COMPILE_READ_RASTER
#endif

#ifndef COMPILE_WRITE_RASTER
  #define COMPILE_WRITE_RASTER(type, unused) \
    template void ImageHdu::writeRaster(const Raster<type, -1>&) const; \
    template void ImageHdu::writeRaster(const Raster<type, 2>&) const; \
    template void ImageHdu::writeRaster(const Raster<type, 3>&) const;
EL_FITSIO_FOREACH_RASTER_TYPE(COMPILE_WRITE_RASTER)
  #undef COMPILE_WRITE_RASTER
#endif

} // namespace FitsIO
} // namespace Euclid
