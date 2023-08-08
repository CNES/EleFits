// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_MEFFILE_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/MefFile.h"

namespace Euclid {
namespace Fits {

template <typename... TActions>
MefFile::MefFile(const std::string& filename, FileMode permission, TActions&&... actions) :
    FitsFile(filename, permission), // FIXME create Primary after strategy is set?
    m_hdus(std::max(1L, Cfitsio::HduAccess::count(m_fptr))), // 1 for create, count() for open
    m_strategy() {
  if (m_permission != FileMode::Read) {
    strategy(CiteEleFits()); // FIXME document
  }
  if constexpr (sizeof...(TActions)) {
    strategy(actions...);
    for (const auto& hdu : *this) {
      m_strategy.opened(hdu);
    }
  }
}

template <class T>
const T& MefFile::access(long index) {
  if (index < 0) { // Backward indexing
    index += hduCount();
  }
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
    m_strategy.accessed(*ptr);
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

template <typename T>
HduSelector<T> MefFile::filter(const HduFilter& filter) {
  return {*this, filter * HduCategory::forClass<T>()};
}

const Strategy& MefFile::strategy() const {
  return m_strategy;
}

Strategy& MefFile::strategy() {
  return m_strategy;
}

template <typename... TActions>
void MefFile::strategy(TActions&&... actions) {
  m_strategy.append(std::forward<TActions>(actions)...);
}

template <typename T>
const ImageHdu& MefFile::appendImageHeader(const std::string& name, const RecordSeq& records) {
  Cfitsio::HduAccess::initImageExtension<T, 0>(m_fptr, name, {});
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created)); // FIXME factorize
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().writeSeq(records);
  return hdu;

  // FIXME inverse dependency (see appendImageExt's FIXME)
}

template <typename T, long N>
const ImageHdu& MefFile::appendNullImage(const std::string& name, const RecordSeq& records, const Position<N>& shape) {
  const auto index = m_hdus.size();
  Position<-1> dynamicShape(shape.begin(), shape.end());
  ImageHdu::Initializer<T> init {static_cast<long>(index), name, records, dynamicShape, nullptr};
  m_strategy.compress(m_fptr, init);
  Cfitsio::HduAccess::initImageExtension<T>(m_fptr, name, shape);
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().writeSeq(records);

  // CFITSIO's fits_write_img_null does not support compression
  if (shapeSize(shape) != 0) {
    VecRaster<T, N> raster(shape);
    if constexpr (std::is_floating_point_v<T>) {
      std::fill(raster.begin(), raster.end(), std::numeric_limits<T>::quiet_NaN());
    } else if (records.has("BLANK")) {
      const auto blank = records.as<T>("BLANK") - offset<T>();
      std::fill(raster.begin(), raster.end(), blank);
    }
    hdu.raster().write(raster);
  }
  // FIXME To CfitsioWrapper and as `bool ImageRaster::fillNull() const`
  // Check status is acceptable (e.g. != 0 if no BLANK keyword or real values)

  // FIXME check offsetting

  // FIXME Fill Raster and pass to appendImage()?

  return hdu;
}

template <typename TRaster>
const ImageHdu& MefFile::appendImage(const std::string& name, const RecordSeq& records, const TRaster& raster) {
  const auto index = m_hdus.size();
  using T = std::decay_t<typename TRaster::Value>;
  Position<-1> dynamicShape(raster.shape().begin(), raster.shape().end());
  ImageHdu::Initializer<T> init {static_cast<long>(index), name, records, dynamicShape, raster.data()};
  m_strategy.compress(m_fptr, init);
  Cfitsio::HduAccess::initImageExtension<typename TRaster::value_type>(m_fptr, name, raster.shape());
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().writeSeq(records);
  hdu.raster().write(raster);
  return hdu;
  // FIXME Is it more efficient to (1) create dataless HDU and then resize and fill data,
  // or (2) first write data and then shift it to accommodate records?
  // For now, we cannot resize uint64 images (CFITSIO bug), so option (1) cannot be tested.
}

template <typename T>
const T& MefFile::appendCopy(const T& hdu) {

  const auto index = m_hdus.size();

  if (hdu.matches(HduCategory::Bintable)) {
    Cfitsio::HduAccess::binaryCopy(hdu.m_fptr, m_fptr);
    m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  } else {
    if (hdu.matches(HduCategory::RawImage) &&
        (m_strategy.m_compression.empty() || hdu.matches(HduCategory::Metadata))) {
      Cfitsio::HduAccess::binaryCopy(hdu.m_fptr, m_fptr);
      m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
    } else {
      // // setting to huge hdu if hdu size > 2^32
      // if (hdu.readSizeInFile() > (1ULL << 32))
      //   Cfitsio::HduAccess::setHugeHdu(m_fptr, true);

      const auto& image = hdu.template as<ImageHdu>();

#define ELEFITS_COPY_HDU(type, name) \
  if (image.readTypeid() == typeid(type)) { \
    appendImage( \
        hdu.readName(), \
        hdu.header().parseAll(KeywordCategory::User), \
        image.raster().template read<type, -1>()); \
  }
      ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_COPY_HDU)
#undef ELEFITS_COPY_HDU
    }
  }

  const auto& copied = access<T>(index);
  m_strategy.accessed(copied); // FIXME is it access or creation?
  return copied;
}

template <typename... TInfos>
const BintableHdu&
MefFile::appendBintableHeader(const std::string& name, const RecordSeq& records, const TInfos&... infos) {
  Cfitsio::HduAccess::initBintableExtension(m_fptr, name, infos...);
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  m_strategy.created(hdu);
  hdu.header().writeSeq(records);
  return hdu;
}

template <typename... TInfos>
const BintableHdu&
MefFile::appendNullBintable(const std::string& name, const RecordSeq& records, long rowCount, const TInfos&... infos) {
  const auto& hdu = appendBintableHeader(name, records, infos...);

  int status = 0;
  fits_insert_rows(m_fptr, 0, rowCount, &status);
  fits_write_nullrows(m_fptr, 1, rowCount, &status);
  if (status != 0) {
    throw FitsError("Cannot write null rows. Error: " + std::to_string(status));
  }
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
  Cfitsio::HduAccess::assignBintableExtension<TColumns, Size>(
      m_fptr,
      name,
      columns); // FIXME doesn't check for column size
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  m_strategy.created(hdu);
  hdu.header().writeSeq(records);
  return hdu;
  // FIXME use appendBintableExt(name, records, columns...) or inverse dependency
}

void MefFile::remove(long index) {
  if (index == 0) {
    primary() = access<ImageHdu>(1);
    remove(1);
  } else {
    Cfitsio::HduAccess::deleteHdu(m_fptr, index + 1);
    auto it = m_hdus.begin() + index; // FIXME won't work with list (or boost::stable_vector)
    m_hdus.erase(it);
    for (; it != m_hdus.end(); ++it) {
      --(it->m_cfitsioIndex);
    }
  }
}

} // namespace Fits
} // namespace Euclid

#endif
