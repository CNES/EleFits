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

#ifndef _ELECFITSIOWRAPPER_HDUWRAPPER_H
#define _ELECFITSIOWRAPPER_HDUWRAPPER_H

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/HduCategory.h"
#include "EleFitsUtils/StringUtils.h"

#include <array>
#include <fitsio.h>
#include <string>
#include <utility> // index_sequence

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
 * @brief Read the number of HDUs in a Fits file.
 * @warning Empty or incomplete HDUs are not counted.
 */
long count(fitsfile* fptr);

/**
 * @brief Get the index of the current HDU.
 */
long currentIndex(fitsfile* fptr);

/**
 * @brief Get the name of the current HDU.
 */
std::string currentName(fitsfile* fptr);

/**
 * @brief Get the version of the current HDU.
 */
long currentVersion(fitsfile* fptr);

/**
 * @brief Get the type of the current HDU (either Image or Bintable).
 * @return Either HduCategory::Image or HduCategory::Bintable.
 * @details
 * The output of this function can be tested for equality, e.g.:
 * \code
 * if (currentType(fptr) == HduCategory::Image) {
 *   ... // An image HDU
 * }
 * \endcode
 */
Fits::HduCategory currentType(fitsfile* fptr);

/**
 * @brief Check whether current HDU is the Primary HDU.
 */
bool currentIsPrimary(fitsfile* fptr);

/**
 * @brief Go to an HDU specified by its index.
 */
bool gotoIndex(fitsfile* fptr, long index);

/**
 * @brief Go to an HDU specified by its name.
 * @param category The desired HDU category: either Any, Image or Bintable
 */
bool gotoName(
    fitsfile* fptr,
    const std::string& name,
    long version = 0,
    Fits::HduCategory category = Fits::HduCategory::Any);

/**
 * @brief Go to an HDU specified by incrementing the index by a given amount.
 */
bool gotoNext(fitsfile* fptr, long step = 1);

/**
 * @brief Go to the Primary HDU.
 */
bool gotoPrimary(fitsfile* fptr);

/**
 * @brief Initialize the Primary HDU if not done.
 */
bool initPrimary(fitsfile* fptr);

/**
 * @brief Write or update HDU name.
 */
bool updateName(fitsfile* fptr, const std::string& name);

/**
 * @brief Write or update HDU version.
 */
bool updateVersion(fitsfile* fptr, long version);

/**
 * @brief Create a new image HDU with empty data unit.
 */
void createMetadataExtension(fitsfile* fptr, const std::string& name);

/**
 * @brief Create a new image HDU with given name, pixel type and shape.
 */
template <typename T, long N = 2>
void initImageExtension(fitsfile* fptr, const std::string& name, const Fits::Position<N>& shape);

/**
 * @brief Write a Raster in a new image HDU.
 */
template <typename TRaster>
void assignImageExtension(fitsfile* fptr, const std::string& name, const TRaster& raster);

/**
 * @brief Create a new binary table HDU with given name and column infos.
 */
template <typename... TInfos>
void initBintableExtension(fitsfile* fptr, const std::string& name, const TInfos&... infos);

/**
 * @brief Create a new binary table HDU with given name and columns.
 */
template <typename... TColumns>
void assignBintableExtension(fitsfile* fptr, const std::string& name, const TColumns&... columns);

/**
 * @brief Create a new binary table HDU with given name and columns.
 * @tparam TTuple A tuple-like type, which should support `std::get`
 * @tparam size The number of elements in the tuple
 * @todo
 * Should we provide more DoFs, like an offset or a full list of indices?
 */
template <typename TTuple, std::size_t size = std::tuple_size<TTuple>::value>
void assignBintableExtension(fitsfile* fptr, const std::string& name, const TTuple& table);

/**
 * @brief Delete the HDU at given index.
 */
void deleteHdu(fitsfile* fptr, long index);

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_HDUWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/HduWrapper.hpp"
#undef _ELECFITSIOWRAPPER_HDUWRAPPER_IMPL
/// @endcond

#endif
