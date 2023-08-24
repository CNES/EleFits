// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ImageRaster.h"

namespace Euclid {
namespace Fits {

ImageRaster::ImageRaster(fitsfile*& fptr, std::function<void(void)> touch, std::function<void(void)> edit) :
    m_fptr(fptr), m_touch(touch), m_edit(edit) {}

const std::type_info& ImageRaster::read_typeid() const {
  m_touch();
  return Cfitsio::ImageIo::read_typeid(m_fptr);
}

long ImageRaster::read_bitpix() const {
  m_touch();
  return Cfitsio::ImageIo::read_bitpix(m_fptr);
}

long ImageRaster::read_size() const {
  return shapeSize(read_shape<-1>());
}

} // namespace Fits
} // namespace Euclid
