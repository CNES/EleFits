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

#include "EL_FitsData/FitsDataFixture.h"


namespace Euclid {
namespace FitsIO {
namespace Test {

SmallRaster::SmallRaster(long input_width, long input_height) :
    VecRaster<float>({input_width, input_height}),
    width(input_width), height(input_height) {
  for(long x=0; x<shape[0]; ++x)
    for(long y=0; y<shape[1]; ++y)
      operator[]({x, y}) = 0.1F * float(y) + float(x);
}

bool SmallRaster::approx(const Raster<float>& other, float tol) const {
  if(other.shape != this->shape)
    return false;
  for(long i=0; i<this->size(); ++i) {
    const auto o = other.data()[i];
    const auto t = this->data()[i];
    const auto rel = (o - t) / t;
    if(rel > 0 && rel > tol)
      return false;
    if(rel < 0 && -rel > tol)
      return false;
  }
  return true;
}

SmallTable::SmallTable() :
    extname("MESSIER"),
    nums { 45, 7, 31 },
    radecs { {56.8500F, 24.1167F}, {268.4667F, -34.7928F}, {10.6833F, 41.2692F} },
    names { "Pleiades", "Ptolemy Cluster", "Andromeda Galaxy" },
    dists_mags { 0.44, 1.6, 0.8, 3.3, 2900.0, 3.4 },
    num_col ({"ID", "", 1}, nums),
    radec_col ({"RADEC", "deg", 1}, radecs),
    name_col ({"NAME", "", 68}, names), //TODO 68?
    dist_mag_col ({"DIST_MAG", "kal", 2}, dists_mags) {
}

}
}
}
