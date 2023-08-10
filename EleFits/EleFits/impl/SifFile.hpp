// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_SIFFILE_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleFits/SifFile.h"

namespace Euclid {
namespace Fits {

template <typename TRaster>
void SifFile::writeAll(const RecordSeq& records, const TRaster& raster) {
  m_raster.reinit<typename TRaster::Value, TRaster::Dim>(raster.shape());
  m_header.writeSeq(records);
  m_raster.write(raster);
}

template <typename T, long N>
VecRaster<T, N> SifFile::readRaster() const {
  return Cfitsio::ImageIo::read_raster<T, N>(m_fptr);
}

template <typename TRaster>
void SifFile::writeRaster(const TRaster& raster) const {
  Cfitsio::HduAccess::goto_primary(m_fptr); // FXME needed?
  Cfitsio::ImageIo::update_type_shape<typename TRaster::Value, TRaster::Dim>(m_fptr, raster.shape());
  Cfitsio::ImageIo::write_raster(m_fptr, raster);
}

} // namespace Fits
} // namespace Euclid

#endif
