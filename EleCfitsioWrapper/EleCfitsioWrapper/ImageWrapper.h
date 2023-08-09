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
Fits::Position<N> read_shape(fitsfile* fptr);

/**
 * @brief Know if the current image HDU is compressed
*/
inline bool is_compressed(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, long N = 2>
void update_shape(fitsfile* fptr, const Fits::Position<N>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, long N = 2>
Fits::VecRaster<T, N> read_raster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster.
 */
template <typename TRaster>
void read_raster_to(fitsfile* fptr, TRaster& destination);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing subraster.
 */
template <typename T, long N = 2, typename TContainer>
void read_raster_to(fitsfile* fptr, Fits::Subraster<T, N, TContainer>& destination);

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, long M, long N>
Fits::VecRaster<T, M> read_region(fitsfile* fptr, const Fits::Region<N>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param destination The destination raster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination raster.
 */
template <typename TRaster, long N>
void read_region_to(fitsfile* fptr, const Fits::Region<N>& region, TRaster& destination);

/**
 * @brief Read a region of the current image HDU into a pre-existing subraster.
 * @param region The source region
 * @param destination The destination subraster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination subraster.
 */
template <typename T, long M, long N, typename TContainer>
void read_region_to(fitsfile* fptr, const Fits::Region<N>& region, Fits::Subraster<T, M, TContainer>& destination);

/**
 * @brief Write a whole raster in the current image HDU.
 */
template <typename TRaster>
void write_raster(fitsfile* fptr, const TRaster& raster);

/**
 * @brief Write a whole raster into a region of the current image HDU.
 * @param raster The raster to be written
 * @param destination The destination position (size is deduced from raster size)
 */
template <typename TRaster, long N>
void write_region(fitsfile* fptr, const TRaster& raster, const Fits::Position<N>& destination);

/**
 * @brief Write a subraster into a region of the current image HDU.
 * @param subraster The subraster to be written
 * @param destination The destination position (size is deduced from subraster size)
 */
template <typename T, long M, long N, typename TContainer> // FIXME where's M?
void write_region(
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
