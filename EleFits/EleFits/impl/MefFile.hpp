// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_MEFFILE_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleFits/MefFile.h"

namespace Euclid {
namespace Fits {

template <class T>
const T& MefFile::access(long index) {
  Cfitsio::HduAccess::gotoIndex(m_fptr, index + 1); // CFITSIO index is 1-based
  const auto hduType = Cfitsio::HduAccess::currentType(m_fptr);
  auto& ptr = m_hdus[index];
  if (ptr == nullptr) {
    if (hduType == HduCategory::Image) {
      ptr.reset(new ImageHdu(Hdu::Token {}, m_fptr, index));
    } else if (hduType == HduCategory::Bintable) {
      ptr.reset(new BintableHdu(Hdu::Token {}, m_fptr, index));
    } else {
      ptr.reset(new Hdu(Hdu::Token {}, m_fptr, index));
    }
  }
  return ptr->as<T>();
}

template <class T>
const T& MefFile::find(const std::string& name, long version) {
  Cfitsio::HduAccess::gotoName(m_fptr, name, version, HduCategory::forClass<T>());
  return access<T>(Cfitsio::HduAccess::currentIndex(m_fptr) - 1); // -1 because CFITSIO index is 1-based
}

template <class T>
const T& MefFile::access(const std::string& name, long version) {
  const auto category = HduCategory::forClass<T>();
  const Hdu* hduPtr = nullptr;
  for (long i = 0; i < hduCount(); ++i) {
    const auto& hdu = access<Hdu>(i);
    const bool cMatch = (category == HduCategory::Any || hdu.type() == category);
    const bool cnMatch = cMatch && (name == "" || hdu.readName() == name);
    const bool cnvMatch = cnMatch && (version == 0 || hdu.readVersion() == version);
    if (cnvMatch) {
      if (hduPtr) {
        throw FitsError("Several HDU matches."); // TODO specific exception?
      } else {
        hduPtr = &hdu; // FIXME Cppcheck reports "Using object that is out of scope."
      }
    }
  }
  if (not hduPtr) {
    throw FitsError("No HDU match."); // TODO specific exception?
  }
  return hduPtr->as<T>();
}

template <typename THdu>
HduSelector<THdu> MefFile::filter(const HduFilter& filter) {
  return {*this, filter * HduCategory::forClass<THdu>()};
}

template <typename T>
const ImageHdu& MefFile::appendImageHeader(const std::string& name, const RecordSeq& records) {
  Cfitsio::HduAccess::initImageExtension<T, 0>(m_fptr, name, {});
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  hdu.header().writeSeq(records);
  return hdu;

  // FIXME inverse dependency (see appendImageExt's FIXME)
}

template <typename T, long N>
const ImageHdu& MefFile::appendNullImage(const std::string& name, const RecordSeq& records, const Position<N>& shape) {
  Cfitsio::HduAccess::initImageExtension<T>(m_fptr, name, shape);
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  hdu.header().writeSeq(records);

  int status = 0;
  fits_write_img_null(m_fptr, 0, 1, shapeSize(shape), &status);
  // FIXME To CfitsioWrapper and as `bool ImageRaster::fillNull() const`
  // Check status is acceptable (e.g. != 0 if no BLANK keyword or real values)

  // FIXME Fill Raster and pass to appendImage()?

  return hdu;
}

template <typename TRaster>
const ImageHdu& MefFile::appendImage(const std::string& name, const RecordSeq& records, const TRaster& raster) {
  Cfitsio::HduAccess::initImageExtension<typename TRaster::value_type>(m_fptr, name, raster.shape());
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  hdu.header().writeSeq(records);
  hdu.raster().write(raster);
  return hdu;
  // FIXME Is it more efficient to (1) create dataless HDU and then resize and fill data,
  // or (2) first write data and then shift it to accommodate records?
  // For now, we cannot resize uint64 images (CFITSIO bug), so option (1) cannot be tested.
}

template <typename... TInfos>
const BintableHdu&
MefFile::appendBintableHeader(const std::string& name, const RecordSeq& records, const TInfos&... infos) {
  Cfitsio::HduAccess::initBintableExtension(m_fptr, name, infos...);
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  hdu.header().writeSeq(records);
  return hdu;
}

template <typename... TInfos>
const BintableHdu&
MefFile::appendNullBintable(const std::string& name, const RecordSeq& records, long rowCount, const TInfos&... infos) {
  const auto& hdu = appendBintableHeader(name, records, infos...);

  int status = 0;
  fits_write_nullrows(m_fptr, 1, rowCount, &status);
  // FIXME To CfitsioWrapper and as `BintableRows::fillNull(Segment) const;`

  return hdu;
}

template <typename... TColumns>
const BintableHdu&
MefFile::appendBintable(const std::string& name, const RecordSeq& records, const TColumns&... columns) {
  const auto& hdu = appendBintableHeader(name, records, columns.info()...);
  hdu.columns().writeSeq(std::forward_as_tuple(columns...)); // FIXME rm forwarding => should accept single column
  return hdu;
}

template <typename TColumns, std::size_t Size>
const BintableHdu& MefFile::appendBintable(const std::string& name, const RecordSeq& records, const TColumns& columns) {
  Cfitsio::HduAccess::assignBintableExtension<TColumns, Size>(m_fptr, name, columns);
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  hdu.header().writeSeq(records);
  return hdu;
  // FIXME use appendBintableExt(name, records, columns...) or inverse dependency
}

template <typename T, long N>
const ImageHdu& MefFile::initImageExt(const std::string& name, const Position<N>& shape) {
  return appendNullImage<T>(name, {}, shape);
}

template <typename TRaster>
const ImageHdu& MefFile::assignImageExt(const std::string& name, const TRaster& raster) {
  return appendImage(name, {}, raster);
}

template <typename... TInfos>
const BintableHdu& MefFile::initBintableExt(const std::string& name, const TInfos&... infos) {
  return appendBintableHeader(name, {}, infos...);
}

template <typename... TColumns>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const TColumns&... columns) {
  return appendBintable(name, {}, std::forward_as_tuple(columns...));
}

template <typename TColumns, std::size_t count>
const BintableHdu& MefFile::assignBintableExt(const std::string& name, const TColumns& columns) {
  return appendBintable(name, {}, columns);
}

#ifndef DECLARE_ASSIGN_IMAGE_EXT
#define DECLARE_ASSIGN_IMAGE_EXT(type, unused) \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, -1>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 2>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const PtrRaster<type, 3>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, -1>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 2>&); \
  extern template const ImageHdu& MefFile::assignImageExt(const std::string&, const VecRaster<type, 3>&);
ELEFITS_FOREACH_RASTER_TYPE(DECLARE_ASSIGN_IMAGE_EXT)
#undef DECLARE_ASSIGN_IMAGE_EXT
#endif

} // namespace Fits
} // namespace Euclid

#endif
