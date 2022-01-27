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

#include "EleCfitsioWrapper/ImageWrapper.h"

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/Raster.h" // ELEFITS_FOREACH_RASTER_TYPE

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
  ELEFITS_FOREACH_RASTER_TYPE(RETURN_TYPEID_IF_MATCH)
  throw Fits::FitsError("Unknown BITPIX: " + std::to_string(bitpix));
}

template <>
Fits::Position<-1> readShape<-1>(fitsfile* fptr) {
  int status = 0;
  int naxis = 0;
  fits_get_img_dim(fptr, &naxis, &status);
  Fits::Position<-1> shape(naxis);
  fits_get_img_size(fptr, naxis, &shape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read raster shape");
  return shape;
}

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid
