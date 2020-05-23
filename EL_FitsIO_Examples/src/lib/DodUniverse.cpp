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

#include "EL_FitsIO_Examples/DodUniverse.h"

namespace Euclid {
namespace FitsIO {
namespace DataOriented {

void Universe::random(std::size_t count, long width, long height) {
  pos_type<2> shape = {21, 21}; //TODO
  const auto size = shape[0] * shape[1];
  m_data = std::vector<float>(size * count, 1.F);
  for(std::size_t i=0; i<count; ++i) {
    const auto begin = m_data.data() + i * size;
    const int ra = i * width / count;
    const int dec = i * height / count;
    m_sources.push_back({ ra, dec, PtrRaster<float>(shape, begin) });
  }
}

void Universe::load(std::string filename) {
  //TODO avoid append, read rasters in place
  // Requires a funtsion read_raster(const T* begin)
  // and a function read_shape()
  throw(std::runtime_error("Universe loading not yet implemented."));
}

void Universe::append(Source source) {
  const auto begin = source.thumbnail.data();
  const auto end = begin + source.thumbnail.size();
  m_data.insert(m_data.end(), begin, end);
  m_sources.push_back(std::move(source));
}

const std::vector<Source>& Universe::sources() const {
  return m_sources;
}

}
}
}
