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
  const std::size_t size = count * 21 * 21; //TODO variable?
  m_data.resize(size);
  m_sources.resize(count);
  auto d = m_data.data();
  Galaxy g;
  for(std::size_t i=0; i<count; ++i) {
    g.random(i);
    const auto& shape = g.shape();
    g.fill(d, shape);
    m_sources[i] = Source(g.coordinates(), PtrRaster<float>(shape, d));
    d += shape[0] * shape[1];
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
  for(std::size_t i=2; i<=count; ++i) {
    const auto& ext = file.access<ImageHdu>(i);
    const auto ra = ext.parse_record<float>("RA");
    const auto dec = ext.parse_record<float>("DEC");
    const auto raster = ext.read_raster<float>();
    const auto& shape = raster.shape;
    size = shape[0] * shape[1];
    memcpy(d, raster.data(), size * sizeof(float));
    m_sources.push_back({{ra, dec}, PtrRaster<float>(shape, d)});
    d += size;
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

}
}
}
}
