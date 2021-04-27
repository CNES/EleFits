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

#if defined(_EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EL_CfitsioWrapper/ImageWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Image {

/**
 * @brief Variable dimension case.
 */
template <>
FitsIO::Position<-1> readShape<-1>(fitsfile* fptr);

template <long n = 2>
FitsIO::Position<n> readShape(fitsfile* fptr) {
  FitsIO::Position<n> shape;
  int status = 0;
  fits_get_img_size(fptr, n, &shape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read raster shape");
  return shape;
}

template <typename T, long n>
void updateShape(fitsfile* fptr, const FitsIO::Position<n>& shape) {
  int status = 0;
  auto nonconstShape = shape;
  fits_resize_img(fptr, TypeCode<T>::bitpix(), shape.size(), nonconstShape.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot reshape raster");
}

template <typename T, long n>
FitsIO::VecRaster<T, n> readRaster(fitsfile* fptr) {
  int status = 0;
  FitsIO::VecRaster<T, n> raster(readShape<n>(fptr));
  const auto size = raster.size();
  fits_read_img(fptr, TypeCode<T>::forImage(), 1, size, nullptr, raster.data(), nullptr, &status);
  // Number 1 is a 1-based index (so we read the whole raster here)
  CfitsioError::mayThrow(status, fptr, "Cannot read raster");
  return raster;
}

template <typename T, long n>
void writeRaster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster) {
  mayThrowReadonlyError(fptr);
  int status = 0;
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  std::vector<T> nonconstData(begin, end); // const-correctness issue
  fits_write_img(fptr, TypeCode<T>::forImage(), 1, raster.size(), nonconstData.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write raster");
}

} // namespace Image
} // namespace Cfitsio
} // namespace Euclid

#endif
