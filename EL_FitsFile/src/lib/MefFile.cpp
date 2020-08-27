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

#include "EL_FitsFile/MefFile.h"

#include "EL_CfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace FitsIO {

MefFile::MefFile(std::string filename, Permission permission) :
    FitsFile(filename, permission),
    m_hdus(std::max(1L, Cfitsio::Hdu::count(m_fptr))) {} // 1 for create, count() for open

long MefFile::hdu_count() const {
  return m_hdus.size();
}

std::vector<std::string> MefFile::hdu_names() {
  const long count = hdu_count();
  std::vector<std::string> names(count);
  for(long i=0; i<count; ++i)
    names[i] = access<>(i+1).name();
  return names;
}

const RecordHdu& MefFile::init_record_ext(std::string name) {
  Cfitsio::Hdu::create_metadata_extension(m_fptr, name);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::unique_ptr<RecordHdu>(new RecordHdu(m_fptr, size+1)));
  return *m_hdus[size].get();
}

#ifndef COMPILE_ASSIGN_IMAGE_EXT
#define COMPILE_ASSIGN_IMAGE_EXT(T, n) \
  template const ImageHdu& MefFile::assign_image_ext<T, n>(std::string, const Raster<T, n>&);
COMPILE_ASSIGN_IMAGE_EXT(char, 2)
COMPILE_ASSIGN_IMAGE_EXT(int, 2)
COMPILE_ASSIGN_IMAGE_EXT(float, 2)
COMPILE_ASSIGN_IMAGE_EXT(double, 2)
COMPILE_ASSIGN_IMAGE_EXT(char, 3)
COMPILE_ASSIGN_IMAGE_EXT(int, 3)
COMPILE_ASSIGN_IMAGE_EXT(float, 3)
COMPILE_ASSIGN_IMAGE_EXT(double, 3)
#undef COMPILE_ASSIGN_IMAGE_EXT
#endif

}
}
