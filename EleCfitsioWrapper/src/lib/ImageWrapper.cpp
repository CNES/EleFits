// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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

const std::type_info& read_typeid(fitsfile* fptr) {
  int status = 0;
  int bitpix = 0;
  fits_get_img_equivtype(fptr, &bitpix, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read image type");
  ELEFITS_FOREACH_RASTER_TYPE(RETURN_TYPEID_IF_MATCH)
  throw Fits::FitsError("Unknown BITPIX: " + std::to_string(bitpix));
}

long read_bitpix(fitsfile* fptr) {
  int status = 0;
  int bitpix = 0;
  fits_get_img_type(fptr, &bitpix, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read image type");
  return bitpix;
}

template <>
Fits::Position<-1> read_shape<-1>(fitsfile* fptr) {
  int status = 0;
  int naxis = 0;
  fits_get_img_dim(fptr, &naxis, &status);
  Fits::Position<-1> shape(naxis);
  fits_get_img_size(fptr, naxis, &shape[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot read raster shape");
  return shape;
}

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid
