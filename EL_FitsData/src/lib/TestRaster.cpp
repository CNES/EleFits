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

#include "EL_FitsData/TestRaster.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

SmallRaster::SmallRaster(long rasterWidth, long rasterHeight) :
    VecRaster<float>({ rasterWidth, rasterHeight }),
    width(rasterWidth),
    height(rasterHeight) {
  for (long x = 0; x < shape[0]; ++x) {
    for (long y = 0; y < shape[1]; ++y) {
      operator[]({ x, y }) = 0.1F * float(y) + float(x);
    }
  }
}

bool SmallRaster::approx(const Raster<float> &other, float tol) const {
  if (other.shape != this->shape) {
    return false;
  }
  for (long i = 0; i < this->size(); ++i) {
    const auto o = other.data()[i];
    const auto t = this->data()[i];
    const auto rel = (o - t) / t;
    if (rel > 0 && rel > tol) {
      return false;
    }
    if (rel < 0 && -rel > tol) {
      return false;
    }
  }
  return true;
}

} // namespace Test
} // namespace FitsIO
} // namespace Euclid
