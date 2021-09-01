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

#if defined(_EL_FITSFILE_SIFFILE_IMPL) || defined(CHECK_QUALITY)

  #include "EL_FitsFile/SifFile.h"

namespace Euclid {
namespace FitsIO {

template <typename T, long n>
VecRaster<T, n> SifFile::readRaster() const {
  return Cfitsio::Image::readRaster<T, n>(m_fptr);
}

template <typename T, long n>
void SifFile::writeRaster(const Raster<T, n>& raster) const {
  Cfitsio::HduAccess::gotoPrimary(m_fptr);
  Cfitsio::Image::updateTypeShape<T, n>(m_fptr, raster.shape);
  Cfitsio::Image::writeRaster(m_fptr, raster);
}

} // namespace FitsIO
} // namespace Euclid

#endif
