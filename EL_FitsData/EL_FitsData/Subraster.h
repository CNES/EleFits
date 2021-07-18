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

#include <algorithm> // accumulate
#include <functional> // multiplies

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief A _n_-D rectangle region, defined by its first and last positions (both inclusive),
 * or first position and shape.
 * @details
 * Like `Position`, this class stores no pixel values, but coordinates.
 */
template <long n = 2>
struct Region {
  /**
   * @brief Create a region from a first position and shape.
   */
  static Region<n> fromShape(Position<n> firstPosition, Position<n> shape) {
    Region<n> region { firstPosition, firstPosition };
    for (std::size_t i = 0; i < region.first.size(); ++i) { // TODO iterators
      region.last[i] += shape[i] - 1;
    }
    return region;
  }

  /**
   * @brief Compute the region shape.
   */
  Position<n> shape() const {
    auto res = last;
    for (long i = 0; i < dimension(); ++i) { // TODO transform
      res[i] -= first[i] - 1;
    }
    return res;
  }

  /**
   * @brief Get the number of axes.
   */
  long dimension() const {
    return first.size();
  }

  /**
   * @brief Compute the region size.
   */
  long size() const {
    const auto s = shape();
    return std::accumulate(s.begin(), s.end(), 1L, std::multiplies<long>());
  }

  /**
   * @brief The first position in the region.
   */
  Position<n> first;

  /**
   * @brief The last position in the region.
   */
  Position<n> last;
};

/**
 * @brief A helper class to screen a `Region`.
 */
template <long n = 2>
class RegionScreener {
public:
  /**
   * @brief Constructor.
   * @param region The region to be screened
   * @param followers Positions which follow the same moves as the current position
   */
  RegionScreener(const Region<n>& region, const std::vector<Position<n>>& followers = {}) :
      m_region(region), m_current(region.first), m_firsts(followers), m_followers(followers) {}

  /**
   * @brief Get the current position.
   */
  const Position<n>& current() const {
    return m_current;
  }

  /**
   * @brief Get the followers positions.
   */
  const std::vector<Position<n>>& followers() const {
    return m_followers;
  }

  /**
   * @brief Reset the current and followers positions to the initial positions.
   */
  void reset() {
    m_current = m_region.first;
    for (std::size_t i = 0; i < m_followers.size(); ++i) {
      m_followers[i] = m_firsts[i];
    }
  }

  /**
   * @brief Update and get the current position and that of a follower.
   * @details
   * Move the current position by 1 pixel,
   * such that the corresponding index in a `Raster` would be increased to the next one.
   * @warning
   * The function follows a modulo arithmetics:
   * `next(last()) = first()`
   */
  const Position<n>& next() {
    if (m_current == m_region.last) {
      m_current = m_region.first;
      m_followers = m_firsts;
      return m_current;
    }
    m_current[0]++;
    for (auto& f : m_followers) {
      f[0]++;
    }
    for (std::size_t i = 0; i < m_current.size(); ++i) {
      if (m_current[i] > m_region.last[i]) {
        m_current[i] = m_region.first[i];
        m_current[i + 1]++;
        for (std::size_t j = 0; j < m_followers.size(); ++j) {
          m_followers[j][i] = m_firsts[j][i];
          m_followers[j][i + 1]++;
        }
      }
    }
    return m_current;
  }

private:
  const Region<n>& m_region;
  Position<n> m_current;
  std::vector<Position<n>> m_firsts;
  std::vector<Position<n>> m_followers;
};

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
