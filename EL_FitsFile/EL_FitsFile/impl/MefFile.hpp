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

#if defined(_EL_FITSFILE_MEFFILE_IMPL) || defined(CHECK_QUALITY)

  #include "EL_FitsFile/MefFile.h"

namespace Euclid {
namespace FitsIO {

template <class T>
const T& MefFile::access(long index) {
  Cfitsio::HduAccess::gotoIndex(m_fptr, index + 1); // CFitsIO index is 1-based
  const auto hduType = Cfitsio::HduAccess::currentType(m_fptr);
  auto& ptr = m_hdus[index];
  if (ptr == nullptr) {
    if (hduType == HduCategory::Image) {
      ptr.reset(new ImageHdu(RecordHdu::Token {}, m_fptr, index));
    } else if (hduType == HduCategory::Bintable) {
      ptr.reset(new BintableHdu(RecordHdu::Token {}, m_fptr, index));
    } else {
      ptr.reset(new RecordHdu(RecordHdu::Token {}, m_fptr, index));
    }
  }
  return ptr->as<T>();
}

template <class T>
const T& MefFile::accessFirst(const std::string& name, long version) {
  Cfitsio::HduAccess::gotoName(m_fptr, name, version, HduCategory::forClass<T>());
  return access<T>(Cfitsio::HduAccess::currentIndex(m_fptr) - 1); // -1 because CFitsIO index is 1-based
}

template <class T>
const T& MefFile::access(const std::string& name, long version) {
  const auto category = HduCategory::forClass<T>();
  const RecordHdu* hduPtr = nullptr;
  for (long i = 0; i < hduCount(); ++i) {
    const auto& hdu = access<RecordHdu>(i);
    const bool cMatch = (category == HduCategory::Any || hdu.type() == category);
    const bool cnMatch = cMatch && (name == "" || hdu.readName() == name);
    const bool cnvMatch = cnMatch && (version == 0 || hdu.readVersion() == version);
    if (cnvMatch) {
      if (hduPtr) {
        throw FitsIOError("Several HDU matches."); // TODO specific exception?
      } else {
        hduPtr = &hdu;
      }
    }
  }
  if (not hduPtr) {
    throw FitsIOError("No HDU match."); // TODO specific exception?
  }
  return hduPtr->as<T>();
}

template <class T>
const T& MefFile::accessPrimary() {
  return access<T>(MefFile::primaryIndex);
}

template <typename THdu>
MefFile::HduSelector<THdu> MefFile::select(const HduFilter& filter) {
  return { *this, filter * HduCategory::forClass<THdu>() };
}

template <typename T, long n>
const ImageHdu& MefFile::initImageExt(const std::string& name, const Position<n>& shape) {
  Cfitsio::HduAccess::createImageExtension<T, n>(m_fptr, name, shape);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(RecordHdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<ImageHdu>();
}

template <typename T, long n>
const ImageHdu& MefFile::assignImageExt(const std::string& name, const Raster<T, n>& raster) {
  Cfitsio::HduAccess::createImageExtension(m_fptr, name, raster);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(RecordHdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<ImageHdu>();
}

template <typename... Ts>
const BintableHdu& MefFile::initBintableExt(const std::string& name, const ColumnInfo<Ts>&... header) {
  Cfitsio::HduAccess::createBintableExtension(m_fptr, name, header...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(RecordHdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

template <typename... Ts>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const Column<Ts>&... columns) {
  Cfitsio::HduAccess::createBintableExtension(m_fptr, name, columns...);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(RecordHdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

template <typename Tuple, std::size_t count>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const Tuple& columns) {
  Cfitsio::HduAccess::createBintableExtension<Tuple, count>(m_fptr, name, columns);
  const auto size = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(RecordHdu::Token {}, m_fptr, size, HduCategory::Created));
  return m_hdus[size]->as<BintableHdu>();
}

  #ifndef DECLARE_ASSIGN_IMAGE_EXT
    #define DECLARE_ASSIGN_IMAGE_EXT(type, unused) \
      extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const Raster<type, -1>&); \
      extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const Raster<type, 2>&); \
      extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const Raster<type, 3>&);
EL_FITSIO_FOREACH_RASTER_TYPE(DECLARE_ASSIGN_IMAGE_EXT)
    #undef DECLARE_ASSIGN_IMAGE_EXT
  #endif

} // namespace FitsIO
} // namespace Euclid

#endif
