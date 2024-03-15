// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_SIFFILE_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleFits/SifFile.h"

namespace Fits {

template <typename TRaster>
void SifFile::write(const RecordSeq& records, const TRaster& raster)
{
  m_raster.update_type_shape<typename TRaster::Value, TRaster::Dimension>(raster.shape());
  m_header.write_n(records);
  m_raster.write(raster);
}

} // namespace Fits

#endif
