// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_IMAGEWRAPPER_H
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_H

#include "EleFitsData/Raster.h"

#include <fitsio.h>
#include <string>
#include <typeinfo>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Image-related functions.
 */
namespace ImageIo {

/**
 * @brief Read the value type of the current image HDU.
 */
const std::type_info& read_typeid(fitsfile* fptr);

/**
 * @brief Read the cfitsio bitpix value of the current image HDU.
 */
long read_bitpix(fitsfile* fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <long N = 2>
Fits::Position<N> readShape(fitsfile* fptr);

/**
 * @brief Know if the current image HDU is compressed
*/
inline bool is_compressed(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, long N = 2>
void updateShape(fitsfile* fptr, const Fits::Position<N>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, long N = 2>
Fits::VecRaster<T, N> readRaster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster.
 */
template <typename TRaster>
void readRasterTo(fitsfile* fptr, TRaster& destination);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing subraster.
 */
template <typename T, long N = 2, typename TContainer>
void readRasterTo(fitsfile* fptr, Fits::Subraster<T, N, TContainer>& destination);

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, long M, long N>
Fits::VecRaster<T, M> readRegion(fitsfile* fptr, const Fits::Region<N>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param destination The destination raster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination raster.
 */
template <typename TRaster, long N>
void readRegionTo(fitsfile* fptr, const Fits::Region<N>& region, TRaster& destination);

/**
 * @brief Read a region of the current image HDU into a pre-existing subraster.
 * @param region The source region
 * @param destination The destination subraster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination subraster.
 */
template <typename T, long M, long N, typename TContainer>
void readRegionTo(fitsfile* fptr, const Fits::Region<N>& region, Fits::Subraster<T, M, TContainer>& destination);

/**
 * @brief Write a whole raster in the current image HDU.
 */
template <typename TRaster>
void writeRaster(fitsfile* fptr, const TRaster& raster);

/**
 * @brief Write a whole raster into a region of the current image HDU.
 * @param raster The raster to be written
 * @param destination The destination position (size is deduced from raster size)
 */
template <typename TRaster, long N>
void writeRegion(fitsfile* fptr, const TRaster& raster, const Fits::Position<N>& destination);

/**
 * @brief Write a subraster into a region of the current image HDU.
 * @param subraster The subraster to be written
 * @param destination The destination position (size is deduced from subraster size)
 */
template <typename T, long M, long N, typename TContainer> // FIXME where's M?
void writeRegion(
    fitsfile* fptr,
    const Fits::Subraster<T, N, TContainer>& subraster,
    const Fits::Position<N>& destination);

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/ImageWrapper.hpp"
#undef _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
