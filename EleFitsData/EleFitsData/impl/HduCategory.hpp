/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#if defined(_ELEFITSDATA_HDUCATEGORY_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/HduCategory.h"

namespace Euclid {
namespace Fits {

class Hdu;
class Header;
class ImageHdu;
class ImageRaster;
class BintableHdu;
class BintableColumns;

template <>
HduCategory HduCategory::forClass<Hdu>();

template <>
HduCategory HduCategory::forClass<Header>();

template <>
HduCategory HduCategory::forClass<ImageHdu>();

template <>
HduCategory HduCategory::forClass<ImageRaster>();

template <>
HduCategory HduCategory::forClass<BintableHdu>();

template <>
HduCategory HduCategory::forClass<BintableColumns>();

} // namespace Fits
} // namespace Euclid

#endif
