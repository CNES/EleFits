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

#include "EleFitsData/Raster.h"
#include "EleFitsData/Region.h"

#include <algorithm> // accumulate
#include <functional> // multiplies

namespace Euclid {
namespace FitsIO {

/// @cond INTERNAL

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
class Subraster {
public:
  /**
   * @brief Constructor.
   */
  Subraster(const Raster<T, n>& parent, const Region<n>& region) :
      m_cParent(&parent), m_parent(nullptr), m_region(region) {}

  /**
   * @brief Constructor.
   */
  Subraster(Raster<T, n>& parent, const Region<n>& region) : m_cParent(&parent), m_parent(&parent), m_region(region) {}

  /**
   * @brief The subraster shape.
   */
  Position<n> shape() const {
    return m_region.shape();
  }

  /**
   * @brief The number of pixels in the subraster.
   */
  long size() const {
    return m_region.size();
  }

  /**
   * @brief The parent raster.
   */
  const Raster<T, n>& parent() const {
    return *m_cParent;
  }

  /**
   * @copydoc parent()
   */
  Raster<T, n>& parent() {
    return *m_parent;
  }

  /**
   * @brief The region.
   */
  const Region<n>& region() const {
    return m_region;
  }

  /**
   * @brief Pixel at given position.
   */
  const T& operator[](const Position<n>& pos) const {
    return (*m_cParent)[pos + m_region.front];
  }

  /**
   * @brief Pixel at given position.
   */
  T& operator[](const Position<n>& pos) {
    return (*m_parent)[pos + m_region.front];
  }

private:
  /**
   * @brief Read-only pointer to the raster.
   */
  const Raster<T, n>* m_cParent;

  /**
   * @brief Read/write pointer to the raster.
   */
  Raster<T, n>* m_parent;

  /**
   * @brief The region.
   */
  Region<n> m_region;
};

/// @endcond

} // namespace FitsIO
} // namespace Euclid

#endif
