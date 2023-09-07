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
    m_strategy()
{
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

const Strategy& MefFile::strategy() const
{
  return m_strategy;
}

Strategy& MefFile::strategy()
{
  return m_strategy;
}

template <class T>
const T& MefFile::access(long index)
{
  if (index < 0) { // Backward indexing
    index += hdu_count();
  }
  Cfitsio::HduAccess::goto_index(m_fptr, index + 1); // CFITSIO index is 1-based
  const auto hdu_type = Cfitsio::HduAccess::current_type(m_fptr);
  auto& ptr = m_hdus[index];
  if (ptr == nullptr) {
    if (hdu_type == HduCategory::Image) {
      ptr.reset(new ImageHdu(Hdu::Token {}, m_fptr, index));
    } else if (hdu_type == HduCategory::Bintable) {
      ptr.reset(new BintableHdu(Hdu::Token {}, m_fptr, index));
    } else {
      ptr.reset(new Hdu(Hdu::Token {}, m_fptr, index));
    }
    m_strategy.accessed(*ptr);
  }
  return ptr->as<T>();
}

template <class T>
const T& MefFile::find(const std::string& name, long version)
{
  Cfitsio::HduAccess::goto_name(m_fptr, name, version, HduCategory::forClass<T>());
  return access<T>(Cfitsio::HduAccess::current_index(m_fptr) - 1); // -1 because CFITSIO index is 1-based
}

template <class T>
const T& MefFile::access(const std::string& name, long version)
{
  const auto category = HduCategory::forClass<T>();
  const Hdu* hdu_ptr = nullptr;
  for (long i = 0; i < hdu_count(); ++i) {
    const auto& hdu = access<Hdu>(i);
    const bool c_match = (category == HduCategory::Any || hdu.type() == category);
    const bool cn_match = c_match && (name == "" || hdu.read_name() == name);
    const bool cnv_match = cn_match && (version == 0 || hdu.read_version() == version);
    if (cnv_match) {
      if (hdu_ptr) {
        throw FitsError("Several HDU matches."); // TODO specific exception?
      } else {
        hdu_ptr = &hdu; // FIXME Cppcheck reports "Using object that is out of scope."
      }
    }
  }
  if (not hdu_ptr) {
    throw FitsError("No HDU match."); // TODO specific exception?
  }
  return hdu_ptr->as<T>();
}

template <typename T>
HduSelector<T> MefFile::filter(const HduFilter& filter)
{
  return {*this, filter * HduCategory::forClass<T>()};
}

template <typename... TActions>
void MefFile::strategy(TActions&&... actions)
{
  m_strategy.append(std::forward<TActions>(actions)...);
}

template <typename T>
const T& MefFile::append(const T& hdu)
{
  const auto index = m_hdus.size();

  if (hdu.matches(HduCategory::Bintable)) {
    Cfitsio::HduAccess::copy_verbatim(hdu.m_fptr, m_fptr);
    m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  } else {
    if (hdu.matches(HduCategory::RawImage) &&
        (m_strategy.m_compression.empty() || hdu.matches(HduCategory::Metadata))) {
      Cfitsio::HduAccess::copy_verbatim(hdu.m_fptr, m_fptr);
      m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
    } else {
      // // setting to huge hdu if hdu size > 2^32
      // if (hdu.size_in_file() > (1ULL << 32))
      //   Cfitsio::HduAccess::enable_huge_compression(m_fptr, true); // FIXME to append_image

      const auto& image = hdu.template as<ImageHdu>();

#define ELEFITS_COPY_HDU(type, name) \
  if (image.read_typeid() == typeid(type)) { \
    append_image( \
        hdu.read_name(), \
        hdu.header().parse_all(KeywordCategory::User), \
        image.raster().template read<type, -1>()); \
  }
      ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_COPY_HDU)
#undef ELEFITS_COPY_HDU
    }
  }

  const auto& copy = access<T>(index);
  m_strategy.copied(copy);
  return copy;
}

template <typename T>
const ImageHdu& MefFile::append_image_header(const std::string& name, const RecordSeq& records)
{
  Cfitsio::HduAccess::init_image<T, 0>(m_fptr, name, {});
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created)); // FIXME factorize
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().write_n(records);
  return hdu;

  // FIXME inverse dependency (see appendImageExt's FIXME)
}

template <typename T, long N>
const ImageHdu& MefFile::append_null_image(const std::string& name, const RecordSeq& records, const Position<N>& shape)
{
  const auto index = m_hdus.size();
  Position<-1> dynamic_shape(shape.begin(), shape.end());
  ImageHdu::Initializer<T> init {static_cast<long>(index), name, records, dynamic_shape, nullptr};
  m_strategy.compress(m_fptr, init);
  Cfitsio::HduAccess::init_image<T>(m_fptr, name, shape);
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().write_n(records);

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
  // FIXME To CfitsioWrapper and as `bool ImageRaster::fill_null() const`
  // Check status is acceptable (e.g. != 0 if no BLANK keyword or real values)

  // FIXME check offsetting

  // FIXME Fill Raster and pass to append_image()?

  return hdu;
}

template <typename TRaster>
const ImageHdu& MefFile::append_image(const std::string& name, const RecordSeq& records, const TRaster& raster)
{
  const auto index = m_hdus.size();
  using T = std::decay_t<typename TRaster::Value>;
  Position<-1> dynamic_shape(raster.shape().begin(), raster.shape().end());
  ImageHdu::Initializer<T> init {static_cast<long>(index), name, records, dynamic_shape, raster.data()};
  m_strategy.compress(m_fptr, init);
  Cfitsio::HduAccess::init_image<typename TRaster::value_type>(m_fptr, name, raster.shape());
  m_hdus.push_back(std::make_unique<ImageHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<ImageHdu>();
  m_strategy.created(hdu);
  hdu.header().write_n(records);
  hdu.raster().write(raster);
  return hdu;
  // FIXME Is it more efficient to (1) create dataless HDU and then resize and fill data,
  // or (2) first write data and then shift it to accommodate records?
  // For now, we cannot resize uint64 images (CFITSIO bug), so option (1) cannot be tested.
}

template <typename... TInfos>
const BintableHdu&
MefFile::append_bintable_header(const std::string& name, const RecordSeq& records, const TInfos&... infos)
{
  Cfitsio::HduAccess::init_bintable(m_fptr, name, infos...);
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  m_strategy.created(hdu);
  hdu.header().write_n(records);
  return hdu;
}

template <typename... TInfos>
const BintableHdu&
MefFile::append_null_bintable(const std::string& name, const RecordSeq& records, long row_count, const TInfos&... infos)
{
  const auto& hdu = append_bintable_header(name, records, infos...);

  int status = 0;
  fits_insert_rows(m_fptr, 0, row_count, &status);
  fits_write_nullrows(m_fptr, 1, row_count, &status);
  if (status != 0) {
    throw FitsError("Cannot write null rows. Error: " + std::to_string(status));
  }
  // FIXME To CfitsioWrapper and as `BintableRows::fill_null(Segment) const;`

  return hdu;
}

template <typename... TColumns>
const BintableHdu&
MefFile::append_bintable(const std::string& name, const RecordSeq& records, const TColumns&... columns)
{
  const auto& hdu = append_bintable_header(name, records, columns.info()...);
  hdu.columns().write_n(std::forward_as_tuple(columns...)); // FIXME rm forwarding => should accept single column
  return hdu;
}

template <typename TColumns, std::size_t Size>
const BintableHdu& MefFile::append_bintable(const std::string& name, const RecordSeq& records, const TColumns& columns)
{
  Cfitsio::HduAccess::assign_bintable<TColumns, Size>(m_fptr, name,
                                                      columns); // FIXME doesn't check for column size
  const auto index = m_hdus.size();
  m_hdus.push_back(std::make_unique<BintableHdu>(Hdu::Token {}, m_fptr, index, HduCategory::Created));
  const auto& hdu = m_hdus[index]->as<BintableHdu>();
  m_strategy.created(hdu);
  hdu.header().write_n(records);
  return hdu;
  // FIXME use append_bintable(name, records, columns...) or inverse dependency
}

void MefFile::remove(long index)
{
  if (index == 0) {
    primary() = access<ImageHdu>(1);
    remove(1);
  } else {
    Cfitsio::HduAccess::remove(m_fptr, index + 1);
    auto it = m_hdus.begin() + index; // FIXME won't work with list (or boost::stable_vector)
    m_hdus.erase(it);
    for (; it != m_hdus.end(); ++it) {
      --((*it)->m_cfitsio_index);
    }
  }
}

} // namespace Fits
} // namespace Euclid

#endif
