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

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"
#include "EL_FitsData/Raster.h"

#include <fitsio.h>
#include <string>

namespace Euclid {

// FIXME move to Raster.h
namespace FitsIO {

template <typename T, long n = 2>
using NdArray = Raster<T, n>;

template <typename T, long n = 2>
using VecNdArray = VecRaster<T, n>;

template <long n = 2>
using NdIndex = Position<n>;

template <long n = 2>
class NdRegion {
public:
  static NdRegion<n> fromOver(NdIndex<n> first, NdIndex<n> shape) {
    NdRegion<n> region { first, first, shape, first };
    for (std::size_t i = 0; i < region.m_first.size(); ++i) {
      region.m_last[i] = region.m_first[i] + region.m_shape[i] - 1;
      region.m_step[i] = 1;
    }
    return region;
  }

  static NdRegion<n> fromTo(NdIndex<n> first, NdIndex<n> last) {
    NdRegion<n> region { first, last, first, first };
    for (std::size_t i = 0; i < region.m_first.size(); ++i) {
      region.m_shape[i] = region.m_last[i] - region.m_first[i] + 1;
      region.m_step[i] = 1;
    }
    return region;
  }

  NdIndex<n> first() const {
    return m_first;
  }
  NdIndex<n> last() const {
    return m_last;
  }
  NdIndex<n> shape() const {
    return m_shape;
  }
  NdIndex<n> step() const {
    return m_step;
  }

private:
  NdRegion(NdIndex<n> first, NdIndex<n> last, NdIndex<n> shape, NdIndex<n> step) :
      m_first(first), m_last(last), m_shape(shape), m_step(step) {}
  NdIndex<n> m_first;
  NdIndex<n> m_last;
  NdIndex<n> m_shape;
  NdIndex<n> m_step;
};

template <typename T, long n = 2>
struct NdView {
  VecNdArray<T, n>& parent; // FIXME
  NdRegion<n> region;
};

} // namespace FitsIO

namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Read the value type of the current image HDU.
 */
const std::type_info& readTypeid(fitsfile* fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <long n = 2>
FitsIO::Position<n> readShape(fitsfile* fptr);

/**
 * @brief Reshape the Raster of the current image HDU.
 */
template <typename T, long n = 2>
void updateShape(fitsfile* fptr, const FitsIO::Position<n>& shape);

/**
 * @brief Read a Raster in current image HDU.
 */
template <typename T, long n = 2>
FitsIO::VecRaster<T, n> readRaster(fitsfile* fptr);

/**
 * @brief Write a Raster in current image HDU.
 */
template <typename T, long n = 2>
void writeRaster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster);

/**
 * @brief Read an array region.
 */
template <typename T, long n = 2>
FitsIO::VecNdArray<T, n> readRegion(fitsfile* fptr, const FitsIO::NdRegion<n>& region) {
  FitsIO::VecNdArray<T, n> raster(region.shape());
  int status = 0;
  const std::size_t dim = region.first().size(); // TODO shortcut
  FitsIO::NdIndex<n> first = region.first();
  FitsIO::NdIndex<n> last = region.last();
  std::vector<long> step(dim, 1);
  for (std::size_t i = 0; i < dim; ++i) {
    first[i]++; // CFitsIO is 1-based
    last[i]++; // idem
  }
  fits_read_subset(
      fptr,
      TypeCode<T>::forImage(),
      first.data(),
      last.data(),
      step.data(),
      nullptr,
      raster.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read image region.");
  return raster;
}

namespace Internal {

template <long n>
long countLines(const FitsIO::NdIndex<n>& first, const FitsIO::NdIndex<n>& last) {
  long res = 1;
  for (std::size_t i = 1; i < first.size(); ++i) {
    res *= last[i] - first[i] + 1;
  }
  return res;
}

template <long n>
void incLinePos(
    const FitsIO::NdIndex<n>& first,
    const FitsIO::NdIndex<n>& last,
    FitsIO::NdIndex<n>& src,
    FitsIO::NdIndex<n>& dst) {
  src[0]++;
  dst[0]++;
  for (std::size_t i = 0; i < src.size(); ++i) {
    if (src[i] > last[i]) {
      const auto delta = dst[i] - src[i];
      src[i] = first[i];
      dst[i] = src[i] + delta;
      src[i + 1]++;
      dst[i + 1]++;
    }
  }
}

} // namespace Internal

/**
 * @brief Read a region in place.
 * @details
 * Similarly to a nD-blit operation, this method reads the data line-by-line
 * directly in a destination array.
 */
template <typename T, long n = 2>
void readRegionTo(fitsfile* fptr, const FitsIO::NdRegion<n>& region, FitsIO::NdView<T, n>& destination) {

  FitsIO::NdIndex<n> first = region.first();
  for (auto& e : first) {
    e++; // 1-based
  }
  FitsIO::NdIndex<n> last = region.last();
  for (auto& e : last) {
    e++; // 1-based
  }
  last[0] = first[0]; // Region of dimension n-1, to be convolved by reading lines along axis 0
  const auto lineCount = Internal::countLines<n>(first, last);
  std::vector<long> step(destination.parent.dimension(), 1L);

  FitsIO::NdIndex<n> srcFirst = first;
  FitsIO::NdIndex<n> dstFirst = destination.region.first();

  int status = 0;
  for (long i = 0; i < lineCount; ++i) {
    FitsIO::NdIndex<n> srcLast = srcFirst;
    srcLast[0] += region.shape()[0] - 1;
    fits_read_subset(
        fptr,
        TypeCode<T>::forImage(),
        srcFirst.data(),
        srcLast.data(),
        step.data(),
        nullptr,
        &destination.parent[dstFirst],
        nullptr,
        &status);
    CfitsioError::mayThrow(status, fptr, "Cannot read image region.");
    Internal::incLinePos<n>(first, last, srcFirst, dstFirst);
  }
}

} // namespace Image
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/ImageWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
