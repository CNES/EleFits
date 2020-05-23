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

#include "EL_FitsIO_Examples/Observation.h"

#include <algorithm>
#include <functional>

#include "EL_FitsFile/SifFile.h"

namespace Euclid {
namespace FitsIO {

Observation::Observation(pos_type<2> shape) :
    raster(shape) {
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  std::fill(begin, end, 0.F);
}

void Observation::draw(const Raster<float>& thumbnail, int x, int y) {
  const auto twidth = thumbnail.length<0>();
  const auto theight = thumbnail.length<1>();
  if(x < 0 || x + twidth >= raster.length<0>()) return;
  if(y < 0 || y + theight >= raster.length<1>()) return;
  auto tbegin = thumbnail.data();
  const auto rwidth = raster.length<0>();
  auto rbegin = &raster[{x, y}];
  for(long j=0; j<theight; ++j) {
    std::transform(tbegin, tbegin + twidth, rbegin, rbegin, std::plus<float>());
    tbegin += twidth;
    rbegin += rwidth;
  }
}

void Observation::save(std::string filename) {
  SifFile f(filename, SifFile::Permission::CREATE);
  f.write_raster(raster);
}

}
}