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

#include "EleFits/ImageHdu.h"

#include <functional> // multiplies
#include <numeric> // accumulate

namespace Euclid {
namespace Fits {

ImageHdu::ImageHdu(Token token, fitsfile*& fptr, long index, HduCategory status) :
    Hdu(token, fptr, index, HduCategory::Image, status),
    m_raster(
        m_fptr,
        [&]() {
          touchThisHdu();
        },
        [&]() {
          editThisHdu();
        }) {}

ImageHdu::ImageHdu() :
    Hdu(),
    m_raster(
        m_fptr,
        [&]() {
          touchThisHdu();
        },
        [&]() {
          editThisHdu();
        }) {}

const ImageRaster& ImageHdu::raster() const {
  return m_raster;
}

const std::type_info& ImageHdu::readTypeid() const {
  return m_raster.readTypeid();
}

long ImageHdu::readSize() const {
  return m_raster.readSize();
}

HduCategory ImageHdu::readCategory() const {
  auto cat = Hdu::readCategory();
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
  cat &= HduCategory::RawImage; // TODO check compression when implemented
  return cat;
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
