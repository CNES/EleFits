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

#include "EL_FitsIO_Examples/Observation.h"

#include "EL_FitsFile/MefFile.h"

#include <algorithm>
#include <memory>

namespace Euclid {
namespace FitsIO {
namespace Example {
namespace DataOriented {

Source::Source() :
  ra_dec(0, 0), thumbnail({0, 0}, (float*)nullptr) {}

Source::Source(std::complex<double> input_ra_dec, PtrRaster<float> input_thumbnail) :
    ra_dec(input_ra_dec), thumbnail(input_thumbnail) {}

const std::vector<Source>& Universe::sources() const {
  return m_sources;
}

void Universe::random(std::size_t count) {
  const std::size_t size = count * 21 * 21; //TODO adaptive?
  m_data.resize(size);
  auto d = m_data.data();
  m_sources.reserve(count);
  Galaxy g;
  for(std::size_t i=0; i<count; ++i) {
    g.random(i);
    const auto& t = transform(g.thumbnail(), d);
    d += t.size();
    m_sources.emplace_back(g.coordinates(), t);
  }
}

void Universe::load(std::string filename) {
  MefFile file(filename, MefFile::Permission::READ);
  const std::size_t count = file.hdu_count();
  std::size_t size = 0;
  for(std::size_t i=2; i<=count; ++i) {
    const auto& ext = file.access<RecordHdu>(i);
    const auto width = ext.parse_record<long>("NAXIS1");
    const auto height = ext.parse_record<long>("NAXIS2");
    size += width * height;
  }
  m_data.resize(size);
  auto d = m_data.data();
  m_sources.reserve(count);
  for(std::size_t i=2; i<=count; ++i) {
    const auto& ext = file.access<ImageHdu>(i);
    const auto ra = ext.parse_record<float>("RA");
    const auto dec = ext.parse_record<float>("DEC");
    const auto raster = ext.read_raster<float>();
    const auto& t = transform(raster, d);
    d += t.size();
    m_sources.emplace_back(std::complex<double>(ra, dec), t);
  }
}

PtrRaster<float> Universe::transform(const VecRaster<float>& input, float* data) const {
  const auto& shape = input.shape;
  PtrRaster<float> output({shape[1], shape[0]}, data);
  for(long x=0; x<shape[1]; ++x)
    for(long y=0; y<shape[0]; ++y)
      output[{x, y}] = input[{y, x}];
  return output;
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
  const std::size_t size = m_data.size() * sizeof(float);
  const long width = 1024;
  const long height = (size + width - 1) / width;
  const std::size_t begin = (std::size_t) m_data.data();
  VecRaster<unsigned char> map({width, height});
  for(const auto& s : m_sources) {
    const std::size_t s_begin = (std::size_t) s.thumbnail.data();
    const std::size_t s_end = s_begin + s.thumbnail.size() * sizeof(float);
    for(std::size_t i = s_begin + 1; i < s_end - 1; ++i)
      map.vector()[i - begin] = 1;
  }
  return map;
}

}
}
}
}
