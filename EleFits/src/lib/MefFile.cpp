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

#include "EleFits/MefFile.h"

#include "EleCfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Fits {

MefFile::MefFile(const std::string& filename, FileMode permission) :
    FitsFile(filename, permission), m_hdus(std::max(1L, Cfitsio::HduAccess::count(m_fptr))) {
} // 1 for create, count() for open

long MefFile::hduCount() const {
  return m_hdus.size();
}

std::vector<std::string> MefFile::readHduNames() {
  const long count = hduCount();
  std::vector<std::string> names(count);
  for (long i = 0; i < count; ++i) {
    names[i] = access<>(i).readName();
  }
  return names;
}

std::vector<std::pair<std::string, long>> MefFile::readHduNamesVersions() {
  const long count = hduCount();
  std::vector<std::pair<std::string, long>> namesVersions(count);
  for (long i = 0; i < count; ++i) {
    const auto& hdu = access<>(i);
    const auto n = hdu.readName();
    const auto v = hdu.readVersion();
    namesVersions[i] = std::make_pair(n, v);
  }
  return namesVersions;
}

const Hdu& MefFile::operator[](long index) {
  return access<Hdu>(index);
}

const ImageHdu& MefFile::primary() {
  return access<ImageHdu>(0);
}

const Hdu& MefFile::initRecordExt(const std::string& name) {
  Cfitsio::HduAccess::createMetadataExtension(m_fptr, name);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<Hdu>(Hdu::Token {}, m_fptr, size, HduCategory::Image, HduCategory::Created));
  return *m_hdus[size].get();
}

const long MefFile::primaryIndex;

#ifndef COMPILE_ASSIGN_IMAGE_EXT
#define COMPILE_ASSIGN_IMAGE_EXT(type, unused) \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, -1>&); \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 2>&); \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 3>&); \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, -1>&); \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 2>&); \
  template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 3>&);
ELEFITS_FOREACH_RASTER_TYPE(COMPILE_ASSIGN_IMAGE_EXT)
#undef COMPILE_ASSIGN_IMAGE_EXT
#endif

} // namespace Fits
} // namespace Euclid
