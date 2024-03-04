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
Linx::Position<N> read_shape(fitsfile* fptr);

/**
 * @brief Know if the current image HDU is compressed
*/
inline bool is_compressed(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, long N = 2>
void update_shape(fitsfile* fptr, const Linx::Position<N>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, long N = 2>
Linx::VecRaster<T, N> read_raster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster.
 */
template <typename TRaster>
void read_raster_to(fitsfile* fptr, TRaster& destination);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing patch.
 */
template <typename T, long N = 2, typename TContainer>
void read_raster_to(fitsfile* fptr, typename Linx::Raster<T, N, TContainer>::Tile<N>& destination);

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, long M, long N>
Linx::VecRaster<T, M> read_region(fitsfile* fptr, const Linx::Box<N>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param destination The destination raster
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly into a destination raster.
 */
template <typename TRaster, long N>
void read_region_to(fitsfile* fptr, const Linx::Box<N>& region, TRaster& destination);

/**
 * @brief Read a region of the current image HDU into a pre-existing patch.
 * @param region The source region
 * @param destination The destination patch
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly in a destination patch.
 */
template <typename T, long M, long N, typename TContainer>
void read_region_to(
    fitsfile* fptr,
    const Linx::Box<N>& region,
    typename Linx::Raster<T, M, TContainer>::Tile<M>& destination);

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
void write_region(fitsfile* fptr, const TRaster& raster, const Linx::Position<N>& destination);

/**
 * @brief Write a patch into a region of the current image HDU.
 * @param patch The patch to be written
 * @param destination The destination position (size is deduced from patch size)
 */
template <typename T, long M, long N, typename TContainer> // FIXME where's M?
void write_region(
    fitsfile* fptr,
    const typename Linx::Raster<const T, N, TContainer>::ConstTile& patch,
    const Linx::Position<N>& destination);

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/ImageWrapper.hpp"
#undef _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
