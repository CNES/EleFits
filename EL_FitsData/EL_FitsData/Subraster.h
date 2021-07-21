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

#ifndef _EL_FITSDATA_SUBRASTER_H
#define _EL_FITSDATA_SUBRASTER_H

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/Region.h"

#include <algorithm> // accumulate
#include <functional> // multiplies

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief A subraster as a view of a raster region.
 * @details
 * As opposed to a Raster, values of a Subraster are generally not contiguous in memory:
 * they are piece-wise contiguous only.
 * 
 * When a region is indeed contiguous, it is better to rely on a PtrRaster instead:
 * \code
 * VecRaster<char, 3> raster({ 800, 600, 3 });
 * 
 * // Good :)
 * auto region = Region<3>::fromOver({ 100, 100, 0 }, { 100, 100, 3 });
 * Subraster<char, 3> subraster { raster, region };
 * 
 * // Bad :(
 * auto slice = Region<3>::fromTo({ 0, 0, 1 }, { -1, -1, 1 });
 * Subraster<char, 3> contiguousSubraster { raster, slice };
 * 
 * // Good :)
 * PtrRaster<char, 2> ptrRaster({ 800, 600 }, &raster[{ 0, 0, 1 }]);
 * \endcode
 */
template <typename T, long n = 2>
struct Subraster {

  /**
   * @brief The subraster shape.
   */
  Position<n> shape() const {
    return region.shape();
  }

  /**
   * @brief The number of pixels in the subraster.
   */
  long size() const {
    return region.size();
  }

  /**
   * @brief The parent raster.
   */
  Raster<T, n>& parent;

  /**
   * @brief The region.
   */
  Region<n> region;
};

} // namespace FitsIO
} // namespace Euclid

#endif