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

#ifndef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_H

#include <fitsio.h>
#include <string>

#include "EL_FitsData/Raster.h"

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/FileWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace Image {

/**
 * @brief Read the value type of the current image HDU.
 */
const std::type_info &readTypeid(fitsfile *fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <long n = 2>
FitsIO::Position<n> readShape(fitsfile *fptr);

/**
 * @brief Reshape the Raster of the current image HDU.
 */
template <typename T, long n = 2>
void updateShape(fitsfile *fptr, const FitsIO::Position<n> &shape);

/**
 * @brief Read a Raster in current image HDU.
 */
template <typename T, long n = 2>
FitsIO::VecRaster<T, n> readRaster(fitsfile *fptr);

/**
 * @brief Write a Raster in current image HDU.
 */
template <typename T, long n = 2>
void writeRaster(fitsfile *fptr, const FitsIO::Raster<T, n> &raster);

} // namespace Image
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/ImageWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
