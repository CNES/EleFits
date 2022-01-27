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

#if defined(_ELEFITS_HDU_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFits/Hdu.h"
#include "EleFitsData/RecordVec.h"

namespace Euclid {
namespace Fits {

/**
 * @brief Specialization of `Hdu::as()` for the header unit.
 */
template <>
const Header& Hdu::as() const;

template <typename T>
const T& Hdu::as() const {
  return dynamic_cast<const T&>(*this);
}

} // namespace Fits
} // namespace Euclid

#endif
