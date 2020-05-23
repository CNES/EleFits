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

#include "EL_FitsIO_Examples/OodUniverse.h"

#include "EL_FitsFile/MefFile.h"

namespace Euclid {
namespace FitsIO {
namespace ObjectOriented {

void Universe::random(std::size_t count, long width, long height) {
  pos_type<2> shape = {21, 21}; //TODO
  const auto size = shape[0] * shape[1];
  const std::vector<float> data(size, 1.F);
  for(std::size_t i=0; i<count; ++i) {
    const int ra = i * width / count;
    const int dec = i * height / count;
    m_sources.push_back({ ra, dec, VecRaster<float>(shape, data) });
  }
}

void Universe::load(std::string filename) {
  MefFile file(filename, MefFile::Permission::READ);
  const std::size_t count = file.hdu_count();
  for(std::size_t i=1; i<=count; ++i) {
    const auto& ext = file.access<ImageHdu>(i);
    const auto ra = ext.parse_record<int>("RA");
    const auto dec = ext.parse_record<int>("DEC");
    append({ra, dec, ext.read_raster<float>()});
  }
}

void Universe::append(Source source) {
  m_sources.push_back(std::move(source));
}

const std::vector<Source>& Universe::sources() const {
  return m_sources;
}

}
}
}
