// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ImageRaster.h"

namespace Euclid {
namespace Fits {

ImageRaster::ImageRaster(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc) :
    m_fptr(fptr), m_touch(touchFunc), m_edit(editFunc) {}

const std::type_info& ImageRaster::readTypeid() const {
  m_touch();
  return Cfitsio::ImageIo::readTypeid(m_fptr);
}

long ImageRaster::readSize() const {
  return shapeSize(readShape<-1>());
}

} // namespace Fits
} // namespace Euclid
