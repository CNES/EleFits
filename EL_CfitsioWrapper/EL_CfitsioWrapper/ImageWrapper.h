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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include <fitsio.h>
#include <string>

#include "EL_FitsData/Raster.h"

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"


namespace Euclid {
namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Resize the Raster of the current Image HDU.
 */
template<typename T, long n=2>
void resize(fitsfile* fptr, const FitsIO::pos_type<n>& shape);

/**
 * @brief Read a Raster in current Image HDU.
 */
template<typename T, long n=2>
FitsIO::VecRaster<T, n> read_raster(fitsfile* fptr);

/**
 * @brief Write a Raster in current Image HDU.
 */
template<typename T, long n=2>
void write_raster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T, long n>
void resize(fitsfile* fptr, const FitsIO::pos_type<n>& shape) {
  int status = 0;
  auto nonconst_shape = shape;
  fits_resize_img(fptr, TypeCode<T>::bitpix(), n, nonconst_shape.data(), &status);
}

template<typename T, long n>
FitsIO::VecRaster<T, n> read_raster(fitsfile* fptr) {
  FitsIO::VecRaster<T, n> raster;
  int status = 0;
  fits_get_img_size(fptr, n, &raster.shape[0], &status);
  may_throw_cfitsio_error(status);
  const auto size = raster.size();
  raster.vector().resize(size); //TODO instantiate here directly with right shape
  fits_read_img(fptr, TypeCode<T>::forImage(), 1, size, nullptr, raster.data(), nullptr, &status);
  // Number 1 is a 1-base offset (so we read the whole raster here)
  may_throw_cfitsio_error(status, "Cannot read raster");
  return raster;
}

template<typename T, long n>
void write_raster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster) {
  may_throw_readonly_error(fptr);
  int status = 0;
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  std::vector<T> nonconst_data(begin, end); // const-correctness issue
  fits_write_img(fptr, TypeCode<T>::forImage(), 1, raster.size(), nonconst_data.data(), &status);
  may_throw_cfitsio_error(status, "Cannot write raster");
}

}
}
}

#endif
