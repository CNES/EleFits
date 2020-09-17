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

namespace Euclid {
namespace FitsIO {

template <class T>
const T &MefFile::access(long index) {
  Cfitsio::Hdu::gotoIndex(m_fptr, index);
  auto hduType = Cfitsio::Hdu::currentType(m_fptr);
  auto &ptr = m_hdus[index - 1];
  switch (hduType) {
    case Cfitsio::Hdu::Type::Image:
      ptr.reset(new ImageHdu(m_fptr, index));
      break;
    case Cfitsio::Hdu::Type::Bintable:
      ptr.reset(new BintableHdu(m_fptr, index));
      break;
    default:
      ptr.reset(new RecordHdu(m_fptr, index));
      break;
  }
  return dynamic_cast<T &>(*ptr.get());
  // TODO return pointer to allow nullptr when HDU handler is obsolete?
}

template <class T>
const T &MefFile::accessFirst(const std::string &name) {
  Cfitsio::Hdu::gotoName(m_fptr, name);
  return access<T>(Cfitsio::Hdu::currentIndex(m_fptr));
}

template <class T>
const T &MefFile::accessPrimary() {
  return access<T>(1);
}

template <typename T, long n>
const ImageHdu &MefFile::initImageExt(const std::string &name, const Position<n> &shape) {
  Cfitsio::Hdu::createImageExtension<T, n>(m_fptr, name, shape);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::unique_ptr<RecordHdu>(new ImageHdu(m_fptr, size + 1)));
  return dynamic_cast<ImageHdu &>(*m_hdus[size].get());
}

template <typename T, long n>
const ImageHdu &MefFile::assignImageExt(const std::string &name, const Raster<T, n> &raster) {
  Cfitsio::Hdu::createImageExtension(m_fptr, name, raster);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::unique_ptr<RecordHdu>(new ImageHdu(m_fptr, size + 1)));
  return dynamic_cast<ImageHdu &>(*m_hdus[size].get());
}

template <typename... Ts>
const BintableHdu &MefFile::initBintableExt(const std::string &name, const ColumnInfo<Ts> &... header) {
  Cfitsio::Hdu::createBintableExtension(m_fptr, name, header...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::unique_ptr<RecordHdu>(new BintableHdu(m_fptr, size + 1)));
  return dynamic_cast<BintableHdu &>(*m_hdus[size].get());
}

template <typename... Ts>
const BintableHdu &MefFile::assignBintableExt(const std::string &name, const Column<Ts> &... columns) {
  Cfitsio::Hdu::createBintableExtension(m_fptr, name, columns...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::unique_ptr<RecordHdu>(new BintableHdu(m_fptr, size + 1)));
  return dynamic_cast<BintableHdu &>(*m_hdus[size].get());
}

#ifndef DECLARE_ASSIGN_IMAGE_EXT
#define DECLARE_ASSIGN_IMAGE_EXT(T, n) \
  extern template const ImageHdu &MefFile::assignImageExt<T, n>(const std::string &, const Raster<T, n> &);
DECLARE_ASSIGN_IMAGE_EXT(char, 2)
DECLARE_ASSIGN_IMAGE_EXT(int, 2)
DECLARE_ASSIGN_IMAGE_EXT(float, 2)
DECLARE_ASSIGN_IMAGE_EXT(double, 2)
DECLARE_ASSIGN_IMAGE_EXT(char, 3)
DECLARE_ASSIGN_IMAGE_EXT(int, 3)
DECLARE_ASSIGN_IMAGE_EXT(float, 3)
DECLARE_ASSIGN_IMAGE_EXT(double, 3)
#undef DECLARE_ASSIGN_IMAGE_EXT
#endif

} // namespace FitsIO
} // namespace Euclid
