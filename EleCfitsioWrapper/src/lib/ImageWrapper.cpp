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

#include "EleCfitsioWrapper/ImageWrapper.h"

#include "EleFitsData/Raster.h" // EL_FITSIO_FOREACH_RASTER_TYPE

namespace Euclid {
namespace Cfitsio {
namespace ImageIo {

#define RETURN_TYPEID_IF_MATCH(type, name) \
  if (Euclid::Cfitsio::TypeCode<type>::bitpix() == bitpix) { \
    return typeid(type); \
  }

const std::type_info& readTypeid(fitsfile* fptr) {
  int status = 0;
  int bitpix = 0;
  fits_get_img_equivtype(fptr, &bitpix, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read image type");
  EL_FITSIO_FOREACH_RASTER_TYPE(RETURN_TYPEID_IF_MATCH)
  throw FitsIO::FitsError("Unknown BITPIX: " + std::to_string(bitpix));
}

template <>
FitsIO::Position<-1> readShape<-1>(fitsfile* fptr) {
  int status = 0;
  int naxis = 0;
  fits_get_img_dim(fptr, &naxis, &status);
  FitsIO::Position<-1> shape(naxis);
  fits_get_img_size(fptr, naxis, &shape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read raster shape");
  return shape;
}

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid
