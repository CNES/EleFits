/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#if defined(_ELEFITS_SIFFILE_IMPL) || defined(CHECK_QUALITY)

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
  return Cfitsio::ImageIo::readRaster<T, N>(m_fptr);
}

template <typename TRaster>
void SifFile::writeRaster(const TRaster& raster) const {
  Cfitsio::HduAccess::gotoPrimary(m_fptr); // FXME needed?
  Cfitsio::ImageIo::updateTypeShape<typename TRaster::Value, TRaster::Dim>(m_fptr, raster.shape());
  Cfitsio::ImageIo::writeRaster(m_fptr, raster);
}

} // namespace Fits
} // namespace Euclid

#endif
