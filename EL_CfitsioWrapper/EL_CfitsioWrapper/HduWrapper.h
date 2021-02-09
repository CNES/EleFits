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

#ifndef _EL_CFITSIOWRAPPER_HDUWRAPPER_H
#define _EL_CFITSIOWRAPPER_HDUWRAPPER_H

#include <fitsio.h>
#include <string>

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {

namespace FitsIO {

/**
 * @brief HDU type (ASCII tables not supported).
 */
enum class HduType {
  Image, ///< Image HDU
  Bintable ///< Binary table HDU
};

} // namespace FitsIO

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
namespace Hdu {

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
 * @brief Get the Type of the current HDU.
 */
FitsIO::HduType currentType(fitsfile* fptr);

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
 */
bool gotoName(fitsfile* fptr, const std::string& name);

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
 * @brief Create a new image HDU with empty data unit.
 */
void createMetadataExtension(fitsfile* fptr, const std::string& name);

/**
 * @brief Create a new image HDU with given name, pixel type and shape.
 */
template <typename T, long n = 2>
void createImageExtension(fitsfile* fptr, const std::string& name, const FitsIO::Position<n>& shape);

/**
 * @brief Write a Raster in a new image HDU.
 */
template <typename T, long n = 2>
void createImageExtension(fitsfile* fptr, const std::string& name, const FitsIO::Raster<T, n>& raster);

/**
 * @brief Create a new binary table HDU with given name and column infos.
 */
template <typename... Ts>
void createBintableExtension(fitsfile* fptr, const std::string& name, const FitsIO::ColumnInfo<Ts>&... infos);

/**
 * @brief Create a new binary table HDU with given name and columns.
 */
template <typename... Ts>
void createBintableExtension(fitsfile* fptr, const std::string& name, const FitsIO::Column<Ts>&... columns);

/**
 * @brief Create a new binary table HDU with given name and columns.
 * @tparam Tuple A tuple-like type, which should support `std::get`
 * @tparam size The number of elements in the tuple
 * @todo
 * Should we provide more DoFs, like an offset or a full list of indices?
 */
template <typename Tuple, std::size_t size = std::tuple_size<Tuple>::value>
void createBintableExtension(fitsfile* fptr, const std::string& name, const Tuple& table);

/**
 * @brief Delete the HDU at given index.
 */
void deleteHdu(fitsfile* fptr, long index);

} // namespace Hdu
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_HDUWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/HduWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_HDUWRAPPER_IMPL
/// @endcond

#endif
