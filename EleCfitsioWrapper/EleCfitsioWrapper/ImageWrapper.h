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
Linx::Index read_bitpix(fitsfile* fptr);

/**
 * @brief Read the shape of the current image HDU.
 */
template <Linx::Index N = 2>
Linx::Position<N> read_shape(fitsfile* fptr);

/**
 * @brief Know if the current image HDU is compressed
*/
inline bool is_compressed(fitsfile* fptr);

/**
 * @brief Reshape the current image HDU.
 */
template <typename T, Linx::Index N = 2>
void update_shape(fitsfile* fptr, const Linx::Position<N>& shape);

/**
 * @brief Read the whole raster of the current image HDU.
 */
template <typename T, Linx::Index N = 2>
Linx::Raster<T, N> read_raster(fitsfile* fptr);

/**
 * @brief Read the whole raster of the current image HDU into a pre-existing raster or patch.
 */
template <typename TOut>
void read_raster_to(fitsfile* fptr, TOut& out);

/**
 * @brief Read a region of the current image HDU.
 */
template <typename T, Linx::Index M, Linx::Index N>
Linx::Raster<T, M> read_region(fitsfile* fptr, const Linx::Box<N>& region);

/**
 * @brief Read a region of the current image HDU into a pre-existing raster.
 * @param region The source region
 * @param out The destination raster or patch
 * @details
 * Similarly to a blit operation, this method reads the data line-by-line
 * directly into a destination raster or patch.
 */
template <Linx::Index N, typename TOut>
void read_region_to(fitsfile* fptr, const Linx::Box<N>& region, TOut& out);

/**
 * @brief Write a whole raster in the current image HDU.
 */
template <typename TRaster>
void write_raster(fitsfile* fptr, const TRaster& raster);

/**
 * @brief Write a raster or patch into a region of the current image HDU.
 */
template <Linx::Index N, typename TIn>
void write_region(fitsfile* fptr, const Linx::Box<N>& region, TIn& in);

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/ImageWrapper.hpp"
#undef _ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL
/// @endcond

#endif
