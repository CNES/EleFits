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
        })
{}

ImageHdu::ImageHdu() :
    Hdu(),
    m_raster(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        })
{}

const ImageHdu& ImageHdu::operator=(const ImageHdu& rhs) const
{
  update_name(rhs.read_name());
  header().write_n(rhs.header().parse_all(KeywordCategory::User)); // FIXME others?
#define ELEFITS_COPY_HDU(T, _) \
  if (rhs.read_typeid() == typeid(T)) { \
    const auto r = rhs.raster().template read<T, -1>(); \
    update_type_shape<T, -1>(r.shape()); \
    if (r.size()) { \
      raster().write(r); \
    } \
    return *this; \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_COPY_HDU)
#undef ELEFITS_COPY_HDU
  return *this;
}

const ImageRaster& ImageHdu::raster() const
{
  return m_raster;
}

const std::type_info& ImageHdu::read_typeid() const
{
  return m_raster.read_typeid();
}

long ImageHdu::read_size() const
{
  return m_raster.read_size();
}

HduCategory ImageHdu::category() const
{
  auto cat = Hdu::category();
  if (read_size() == 0) {
    cat &= HduCategory::Metadata;
  } else {
    cat &= HduCategory::Data;
  }
  const auto& id = read_typeid();
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

bool ImageHdu::is_compressed() const
{
  touch();
  return Cfitsio::ImageIo::is_compressed(m_fptr);
}

std::unique_ptr<Compression> ImageHdu::read_compression() const
{
  touch();
  return Cfitsio::ImageCompression::read_parameters(m_fptr);
}

template <>
const ImageRaster& Hdu::as() const
{
  return as<ImageHdu>().raster();
}

} // namespace Fits
} // namespace Euclid
