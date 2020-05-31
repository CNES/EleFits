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

#include "EL_FitsIO_Examples/Observation.h"

#include "EL_FitsFile/MefFile.h"

namespace Euclid {
namespace FitsIO {
namespace Example {
namespace ObjectOriented {

Source::Source() :
  ra_dec(0, 0), thumbnail({0, 0}) {}

Source::Source(std::complex<double> input_ra_dec, VecRaster<float> input_thumbnail) :
    ra_dec(input_ra_dec), thumbnail(input_thumbnail) {}

const std::vector<Source>& Universe::sources() const {
  return m_sources;
}

void Universe::random(std::size_t count) {
  m_sources.reserve(count);
  Galaxy g;
  for(std::size_t i=0; i<count; ++i) {
    g.random(i);
    m_sources.emplace_back(g.coordinates(), g.thumbnail());
  }
}

void Universe::load(std::string filename) {
  MefFile file(filename, MefFile::Permission::READ);
  const std::size_t count = file.hdu_count();
  for(std::size_t i=2; i<=count; ++i) {
    const auto& ext = file.access<ImageHdu>(i);
    const auto ra = ext.parse_record<float>("RA");
    const auto dec = ext.parse_record<float>("DEC");
    m_sources.push_back({{ra, dec}, ext.read_raster<float>()});
  }
}

void Universe::save(std::string filename) const {
  MefFile f(filename, MefFile::Permission::CREATE);
  for(const auto& s : m_sources) {
    const auto ra = s.ra_dec.real();
    const auto dec = s.ra_dec.imag();
    const std::string id = std::to_string(ra) + ',' + std::to_string(dec);
    const auto& ext = f.assign_image_ext<float>(id, s.thumbnail);
    ext.write_record("RA", ra);
    ext.write_record("DEC", dec);
  }
}

VecRaster<unsigned char> Universe::memory_map() const {
  const std::size_t begin = (std::size_t) m_sources[0].thumbnail.data();
  const auto count = m_sources.size();
  const auto& last = m_sources[count - 1].thumbnail;
  const std::size_t end = (std::size_t) last.data() + last.size() * sizeof(float);
  const std::size_t size = end - begin;
  const long width = 1024;
  const long height = (size + width - 1) / width;
  VecRaster<unsigned char> map({width, height});
  unsigned char value = 1;
  for(const auto& s : m_sources) {
    const std::size_t s_begin = (std::size_t) s.thumbnail.data();
    const std::size_t s_end = s_begin + s.thumbnail.size() * sizeof(float);
    for(std::size_t i = s_begin; i < s_end; ++i)
      map.vector()[i - begin] = value;
    value += 2;
  }
  return map;
}

}
}
}
}
