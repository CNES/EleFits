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

} // namespace Test
} // namespace Fits
} // namespace Euclid
