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

#ifndef _ELECFITSIOWRAPPER_IMAGEWRAPPER_H
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_H

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/FileWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/Raster.h"

#include <fitsio.h>
#include <string>

namespace Euclid {

namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace ImageIo {

/**
 * @brief Read the value type of the current image HDU.
 */
const std::type_info& readTypeid(fitsfile* fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <long n = 2>
FitsIO::Position<n> readShape(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, long n = 2>
void updateShape(fitsfile* fptr, const FitsIO::Position<n>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, long n = 2>
FitsIO::VecRaster<T, n> readRaster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster.
 */
template <typename T, long n = 2>
void readRasterTo(fitsfile* fptr, FitsIO::Raster<T, n>& destination);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing subraster.
 */
template <typename T, long n = 2>
void readRasterTo(fitsfile* fptr, FitsIO::Subraster<T, n>& destination);

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, long m, long n>
FitsIO::VecRaster<T, m> readRegion(fitsfile* fptr, const FitsIO::Region<n>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param destination The destination raster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination raster.
 */
template <typename T, long m, long n>
void readRegionTo(fitsfile* fptr, const FitsIO::Region<n>& region, FitsIO::Raster<T, m>& destination);

/**
 * @brief Read a region of the current image HDU into a pre-existing subraster.
 * @param region The source region
 * @param destination The destination subraster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination subraster.
 */
template <typename T, long m, long n>
void readRegionTo(fitsfile* fptr, const FitsIO::Region<n>& region, FitsIO::Subraster<T, m>& destination);

/**
 * @brief Write a whole raster in the current image HDU.
 */
template <typename T, long n = 2>
void writeRaster(fitsfile* fptr, const FitsIO::Raster<T, n>& raster);

/**
 * @brief Write a whole raster into a region of the current image HDU.
 * @param raster The raster to be written
 * @param destination The destination position (size is deduced from raster size)
 */
template <typename T, long m, long n>
void writeRegion(fitsfile* fptr, const FitsIO::Raster<T, m>& raster, const FitsIO::Position<n>& destination);

/**
 * @brief Write a subraster into a region of the current image HDU.
 * @param subraster The subraster to be written
 * @param region The destination position (size is deduced from subraster size)
 */
template <typename T, long m, long n>
void writeRegion(fitsfile* fptr, const FitsIO::Subraster<T, n>& subraster, const FitsIO::Position<n>& destination);

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/ImageWrapper.hpp"
#undef _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
