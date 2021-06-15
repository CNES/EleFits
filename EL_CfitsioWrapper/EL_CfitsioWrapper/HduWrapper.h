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

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace FitsIO {

/**
 * @brief HDU category.
 * @details
 * The enumerators are designed to be used as bitmasks to filter on some properties.
 * For example, an HDU of category Primary is also of category ImageHdu and Hdu, but not of category Ext.
 * 
 * For image HDUs, two categories are defined, which mainly aims at simplifying compression-related features:
 * - Integer-valued for values of integral type;
 * - Real-valued for values of type float or double.
 * 
 * For any HDU, the data unit may be empty (i.e. `NAXIS = 0` or `NAXISn = 0` for at least one axis).
 * This is modeled as category Metadata, as opposed to category Data, which means there are values in the data unit.
 * 
 * They can be combined, e.g. to filter on integer-valued image extensions with non-empty data:
 * \code
 * HduCategory intImageExtWithData = HduCategory::IntImageExt & ~HduCategory::Metadata;
 * \endcode
 * 
 * Shortcuts are provided for common combinations, e.g. `ImageExt = Image | Ext`.
 * 
 * To test categories, bitwise boolean operators (`&`, `|`, `~`) are defined.
 * Additionally, operator <= allows checking whether an HDU category matches a set of categories.
 */
enum HduCategory
{
  Metadata = 0b000'0001, ///< HDU without data
  Data = 0b0000'0010, ///< HDU with data
  Primary = 0b0000'0100, ///< Primary HDU
  MetadataPrimary = Metadata | Primary, ///< Primary HDU without data
  DataPrimary = Data | Primary, ///< Primary HDU with data
  Ext = 0b0000'1000, ///< Extension
  MetadataExt = Metadata | Ext, ///< Extension without data
  DataExt = Data | Ext, ///< Extension with data
  IntImage = 0b0001'0000, ///< Integer-valued image HDU
  FloatImage = 0b0010'0000, ///< Real-valued image HDU
  Image = IntImage | FloatImage, ///< Image HDU
  MetadataImage = Metadata | Image, ///< Image HDU without data
  DataImage = Data | Image, ///< Image HDU with data
  ImageExt = Image | Ext, ///< Image extension
  MetadataImageExt = Metadata | ImageExt, ///< Image extension without data
  DataImageExt = Data | ImageExt, ///< Image extension with data
  Bintable = 0b0100'0000, ///< Binary table HDU
  Any = 0 ///< Any HDU
};

/**
 * @brief Bit-wise OR operator for masking.
 * @see HduCategory
 */
inline HduCategory operator|(HduCategory a, HduCategory b) {
  return static_cast<HduCategory>(static_cast<int>(a) | static_cast<int>(b));
}

/**
 * @brief In-place bit-wise OR operator for masking.
 * @see HduCategory
 */
inline HduCategory& operator|=(HduCategory& a, HduCategory b) {
  a = a | b;
  return a;
}

/**
 * @brief Bit-wise AND operator for masking.
 * @see HduCategory
 */
inline HduCategory operator&(HduCategory a, HduCategory b) {
  return static_cast<HduCategory>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * @brief In-place bit-wise AND operator for masking.
 * @see HduCategory
 */
inline HduCategory& operator&=(HduCategory& a, HduCategory b) {
  a = a & b;
  return a;
}

/**
 * @brief Bit-wise binary NOT operator for masking.
 * @see HduCategory
 */
inline HduCategory operator~(HduCategory a) {
  return static_cast<HduCategory>(~static_cast<int>(a));
}

/**
 * @brief Check whether an input bitmask contains at least the bits of given categories.
 */
template <HduCategory TCategories>
inline bool isInstance(HduCategory input) {
  return (input & TCategories) == TCategories;
}

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
 * @brief Get the type of the current HDU (either Image or Bintable).
 * @return HduCategory::Image or HduCategory::Bintable
 * @details
 * The output of this function can be tested for equality, e.g.:
 * \code
 * if (currentType(fptr) == HduCategory::Image) {
 *   ... // An image HDU
 * }
 * \endcode
 * @see For a finer output, see currentCategories.
 */
FitsIO::HduCategory currentType(fitsfile* fptr);

/**
 * @brief Get the set of categories of the current HDU.
 * @return A combination of HduCategory's
 * @details
 * The output of this function should be tested with Boolean bitwise operators, e.g.:
 * \code
 * const auto cat = currentCategories(fptr);
 * if (cat & HduCategory::Image && cat & HduCategory::Data) {
 *   ... // An image HDU with non-empty data unit
 * }
 * \endcode
 */
FitsIO::HduCategory currentCategories(fitsfile* fptr);

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
