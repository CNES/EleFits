// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_HDUWRAPPER_H
#define _ELECFITSIOWRAPPER_HDUWRAPPER_H

#include "EleFitsData/Column.h"
#include "EleFitsData/HduCategory.h"
#include "EleFitsData/Raster.h"

#include <fitsio.h>
#include <string>
#include <tuple>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief HDU-related functions.
 *
 * @details
 * An HDU can be of two Types (ASCII tables are not supported):
 * * Image
 * * Bintable
 *
 * Getter functions generally apply to the current HDU.
 * Functions to go to an HDU return false if target HDU is already current HDU.
 * Functions to create an HDU append it at the end of the file.
 */
namespace HduAccess {

/**
 * @brief Read the number of HDUs in a FITS file.
 * @warning Empty or incomplete HDUs are not counted.
 */
Linx::Index count(fitsfile* fptr);

/**
 * @brief Get the index of the current HDU.
 */
Linx::Index current_index(fitsfile* fptr);

/**
 * @brief Get the name of the current HDU.
 * @details
 * Look for `EXTNAME` and then `HDUNAME`; return `""` if not found.
 */
std::string current_name(fitsfile* fptr);

/**
 * @brief Get the version of the current HDU.
 * @details
 * Look for `EXTVER` and then `HDUVER`; return 1 if not found.
 */
long current_version(fitsfile* fptr);

/**
 * @brief Get the byte size of the current HDU.
 */
std::size_t current_size(fitsfile* fptr);

/**
 * @brief Get the type of the current HDU (either Image or Bintable).
 * @return Either HduCategory::Image or HduCategory::Bintable.
 * @details
 * The output of this function can be tested for equality, e.g.:
 * \code
 * if (current_type(fptr) == HduCategory::Image) {
 *   ... // An image HDU
 * }
 * \endcode
 */
Fits::HduCategory current_type(fitsfile* fptr); // FIXME return HduType

/**
 * @brief Check whether current HDU is the Primary HDU.
 */
bool current_is_primary(fitsfile* fptr);

/**
 * @brief Go to an HDU specified by its index.
 */
bool goto_index(fitsfile* fptr, Linx::Index index);

/**
 * @brief Go to an HDU specified by its name.
 * @param category The desired HDU category: either Any, Image or Bintable
 */
bool goto_name(
    fitsfile* fptr,
    const std::string& name,
    Linx::Index version = 0,
    Fits::HduCategory category = Fits::HduCategory::Any);

/**
 * @brief Go to an HDU specified by incrementing the index by a given amount.
 */
bool goto_next(fitsfile* fptr, Linx::Index step = 1);

/**
 * @brief Go to the Primary HDU.
 */
bool goto_primary(fitsfile* fptr);

/**
 * @brief Initialize the Primary HDU if not done.
 */
bool init_primary(fitsfile* fptr);

/**
 * @brief Write or update HDU name.
 */
bool update_name(fitsfile* fptr, const std::string& name);

/**
 * @brief Write or update HDU version.
 */
bool update_version(fitsfile* fptr, long version);

/**
 * @brief Create a new image HDU with given name, pixel type and shape.
 */
template <typename T, Linx::Index N = 2>
void init_image(fitsfile* fptr, const std::string& name, const Linx::Position<N>& shape);

/**
 * @brief Write a Raster in a new image HDU.
 */
template <typename TRaster>
void assign_image(fitsfile* fptr, const std::string& name, const TRaster& raster);

/**
 * @brief Create a new binary table HDU with given name and column infos.
 */
template <typename... TInfos>
void init_bintable(fitsfile* fptr, const std::string& name, const TInfos&... infos);

/**
 * @brief Create a new binary table HDU with given name and columns.
 */
template <typename... TColumns>
void assign_bintable(fitsfile* fptr, const std::string& name, const TColumns&... columns);

/**
 * @brief Create a new binary table HDU with given name and columns.
 * @tparam TTuple A tuple-like type, which should support `std::get`
 * @tparam size The number of elements in the tuple
 * @todo
 * Should we provide more DoFs, like an offset or a full list of indices?
 */
template <typename TTuple, std::size_t size = std::tuple_size<TTuple>::value>
void assign_bintable(fitsfile* fptr, const std::string& name, const TTuple& table);

/**
 * @brief Copies everything as binary in the current HDU of src and appends it to dst.
 */
void copy_verbatim(fitsfile* src, fitsfile* dst);

/**
 * @brief Delete the HDU at given index.
 */
void remove(fitsfile* fptr, Linx::Index index);

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_HDUWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/HduWrapper.hpp"
#undef _ELECFITSIOWRAPPER_HDUWRAPPER_IMPL
/// @endcond

#endif
