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

#include "EleFitsData/TestRaster.h"

namespace Euclid {
namespace Fits {
namespace Test {

SmallRaster::SmallRaster(long rasterWidth, long rasterHeight) :
    VecRaster<float>({rasterWidth, rasterHeight}), width(rasterWidth), height(rasterHeight) {
  for (long y = 0; y < shape()[1]; ++y) {
    for (long x = 0; x < shape()[0]; ++x) {
      operator[]({x, y}) = 0.1F * float(y) + float(x);
    }
  }
}

bool SmallRaster::approx(const Raster<float>& other, double tol) const {
  return rasterApprox(*this, other, tol);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
