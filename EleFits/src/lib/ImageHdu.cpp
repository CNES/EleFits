// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ImageHdu.h"

#include "EleCfitsioWrapper/CompressionWrapper.h"

#include <functional> // multiplies
#include <numeric> // accumulate

namespace Euclid {
namespace Fits {

ImageHdu::ImageHdu(Token token, fitsfile*& fptr, long index, HduCategory status) :
    Hdu(token, fptr, index, HduCategory::Image, status),
    m_raster(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        }) {}

ImageHdu::ImageHdu() :
    Hdu(),
    m_raster(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        }) {}

const ImageHdu& ImageHdu::operator=(const ImageHdu& rhs) const {
  update_name(rhs.read_name());
  header().writeSeq(rhs.header().parseAll(KeywordCategory::User)); // FIXME others?
#define ELEFITS_COPY_HDU(T, _) \
  if (rhs.readTypeid() == typeid(T)) { \
    const auto r = rhs.raster().template read<T, -1>(); \
    updateShape<T, -1>(r.shape()); \
    if (r.size()) { \
      raster().write(r); \
    } \
    return *this; \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_COPY_HDU)
#undef ELEFITS_COPY_HDU
  return *this;
}

const ImageRaster& ImageHdu::raster() const {
  return m_raster;
}

const std::type_info& ImageHdu::readTypeid() const {
  return m_raster.readTypeid();
}

long ImageHdu::read_bitpix() const {
  return m_raster.read_bitpix();
}

long ImageHdu::readSize() const {
  return m_raster.readSize();
}

HduCategory ImageHdu::category() const {
  auto cat = Hdu::category();
  if (readSize() == 0) {
    cat &= HduCategory::Metadata;
  } else {
    cat &= HduCategory::Data;
  }
  const auto& id = readTypeid();
  if (id == typeid(float) || id == typeid(double)) {
    cat &= HduCategory::FloatImage;
  } else {
    cat &= HduCategory::IntImage;
  }
  if (Cfitsio::ImageIo::is_compressed(m_fptr)) {
    cat &= HduCategory::CompressedImageExt;
  } else {
    cat &= HduCategory::RawImage;
  }
  return cat;
}

bool ImageHdu::is_compressed() const {
  touch();
  return Cfitsio::ImageIo::is_compressed(m_fptr);
}

std::unique_ptr<Compression> ImageHdu::read_compression() const {
  touch();
  return Cfitsio::ImageCompression::read_parameters(m_fptr);
}

template <>
const ImageRaster& Hdu::as() const {
  return as<ImageHdu>().raster();
}

#ifndef COMPILE_READ_RASTER
#define COMPILE_READ_RASTER(type, unused) \
  template VecRaster<type, -1> ImageHdu::readRaster() const; \
  template VecRaster<type, 2> ImageHdu::readRaster() const; \
  template VecRaster<type, 3> ImageHdu::readRaster() const;
ELEFITS_FOREACH_RASTER_TYPE(COMPILE_READ_RASTER)
#undef COMPILE_READ_RASTER
#endif

#ifndef COMPILE_WRITE_RASTER
#define COMPILE_WRITE_RASTER(type, unused) \
  template void ImageHdu::writeRaster(const PtrRaster<type, -1>&) const; \
  template void ImageHdu::writeRaster(const PtrRaster<type, 2>&) const; \
  template void ImageHdu::writeRaster(const PtrRaster<type, 3>&) const; \
  template void ImageHdu::writeRaster(const VecRaster<type, -1>&) const; \
  template void ImageHdu::writeRaster(const VecRaster<type, 2>&) const; \
  template void ImageHdu::writeRaster(const VecRaster<type, 3>&) const;
ELEFITS_FOREACH_RASTER_TYPE(COMPILE_WRITE_RASTER)
#undef COMPILE_WRITE_RASTER
#endif

} // namespace Fits
} // namespace Euclid
