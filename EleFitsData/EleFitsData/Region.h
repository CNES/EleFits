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

#ifndef _ELEFITSDATA_REGION_H
#define _ELEFITSDATA_REGION_H

#include "EleFitsData/Position.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_data_classes
 * @brief A _n_-D rectangle region, defined by its front and back positions (both inclusive),
 * or front position and shape.
 * @details
 * Like `Position`, this class stores no pixel values, but coordinates.
 */
template <long N = 2>
struct Region {
  /**
   * @brief Create a region from a front position and shape.
   */
  static Region<N> fromShape(Position<N> frontPosition, Position<N> shape) {
    Region<N> region {frontPosition, frontPosition};
    region.back += shape - 1;
    return region;
  }

  /**
   * @brief Create an unlimited region.
   * @details
   * Front and back bounds along each axis are respectively 0 and -1.
   */
  static Region<N> whole() {
    return {Position<N>::zero(), Position<N>::max()};
  }

  /**
   * @brief Compute the region shape.
   */
  Position<N> shape() const {
    return back - front + 1;
  }

  /**
   * @brief Get the number of dimensions.
   */
  long dimension() const {
    return front.size();
  }

  /**
   * @brief Compute the region size, i.e. number of pixels.
   */
  long size() const {
    return shapeSize(shape());
  }

  /**
   * @brief The front position in the region.
   */
  Position<N> front;

  /**
   * @brief The back position in the region.
   */
  Position<N> back;
};

/**
 * @relates Region
 * @brief Check whether two regions are equal.
 */
template <long N = 2>
bool operator==(const Region<N>& lhs, const Region<N>& rhs) {
  return lhs.front == rhs.front && lhs.back == rhs.back;
}

/**
 * @relates Region
 * @brief Check whether two regions are different.
 */
template <long N = 2>
bool operator!=(const Region<N>& lhs, const Region<N>& rhs) {
  return lhs.front != rhs.front || lhs.back != rhs.back;
}

/**
 * @relates Region
 * @brief Add a position.
 */
template <long N = 2>
Region<N>& operator+=(Region<N>& lhs, const Position<N>& rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract a position.
 */
template <long N = 2>
Region<N>& operator-=(Region<N>& lhs, const Position<N>& rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Add a scalar to each coordinate.
 */
template <long N = 2>
Region<N>& operator+=(Region<N>& lhs, long rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract a scalar to each coordinate.
 */
template <long N = 2>
Region<N>& operator-=(Region<N>& lhs, long rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Add 1 to each coordinate.
 */
template <long N = 2>
Region<N>& operator++(Region<N>& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract 1 to each coordinate.
 */
template <long N = 2>
Region<N>& operator--(Region<N>& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @relates Region
 * @brief Return the current region and then add 1 to each coordinate.
 */
template <long N = 2>
Region<N> operator++(Region<N>& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @relates Region
 * @brief Return the current region and then subtract 1 to each coordinate.
 */
template <long N = 2>
Region<N> operator--(Region<N>& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @relates Region
 * @brief Identity.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& rhs) {
  return rhs;
}

/**
 * @relates Region
 * @brief Change the sign of each coordinate.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& rhs) {
  return {-rhs.front, -rhs.back};
}

/**
 * @relates Region
 * @brief Add a region and a position.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Region
 * @brief Subtract a region and a position.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @relates Region
 * @brief Add a region and a scalar.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Region
 * @brief Subtract a region and a scalar.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/// @cond
// FIXME replace RegionScreener with PositionIterator?

/**
 * @ingroup image_data_classes
 * @brief A helper class to screen a `Region`.
 */
template <long N = 2>
class RegionScreener {
public:
  /**
   * @brief Constructor.
   * @param region The region to be screened
   * @param followers Positions which follow the same moves as the current position
   */
  RegionScreener(const Region<N>& region, const std::vector<Position<N>>& followers = {}) :
      m_region(region), m_current(region.front), m_fronts(followers), m_followers(followers) {}

  /**
   * @brief Get the current position.
   */
  const Position<N>& current() const {
    return m_current;
  }

  /**
   * @brief Get the followers positions.
   */
  const std::vector<Position<N>>& followers() const {
    return m_followers;
  }

  /**
   * @brief Reset the current and followers positions to the initial positions.
   */
  void reset() {
    m_current = m_region.front;
    m_followers = m_fronts;
  }

  /**
   * @brief Update and get the current position and that of a follower.
   * @details
   * Move the current position by 1 pixel,
   * such that the corresponding index in a `Raster` would be increased to the next one.
   * @warning
   * The function follows a modulo arithmetics:
   * `next(back()) = front()`
   */
  const Position<N>& next() {
    if (m_current == m_region.back) {
      m_current = m_region.front;
      m_followers = m_fronts;
      return m_current;
    }
    m_current[0]++;
    for (auto& f : m_followers) {
      f[0]++;
    }
    for (std::size_t i = 0; i < m_current.size(); ++i) {
      if (m_current[i] > m_region.back[i]) {
        m_current[i] = m_region.front[i];
        m_current[i + 1]++;
        for (std::size_t j = 0; j < m_followers.size(); ++j) {
          m_followers[j][i] = m_fronts[j][i];
          m_followers[j][i + 1]++;
        }
      }
    }
    return m_current;
  }

private:
  const Region<N>& m_region;
  Position<N> m_current;
  std::vector<Position<N>> m_fronts;
  std::vector<Position<N>> m_followers;
};

/// @endcond

} // namespace Fits
} // namespace Euclid

#endif
