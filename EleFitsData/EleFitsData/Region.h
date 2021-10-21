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
template <long n = 2>
struct Region {
  /**
   * @brief Create a region from a front position and shape.
   */
  static Region<n> fromShape(Position<n> frontPosition, Position<n> shape) {
    Region<n> region { frontPosition, frontPosition };
    region.back += shape - 1;
    return region;
  }

  /**
   * @brief Create an unlimited region.
   * @details
   * Front and back bounds along each axis are respectively 0 and -1.
   */
  static Region<n> whole() {
    return { Position<n>::zero(), Position<n>::max() };
  }

  /**
   * @brief Compute the region shape.
   */
  Position<n> shape() const {
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
  Position<n> front;

  /**
   * @brief The back position in the region.
   */
  Position<n> back;
};

/**
 * @brief Check whether two regions are equal.
 */
template <long n = 2>
bool operator==(const Region<n>& lhs, const Region<n>& rhs) {
  return lhs.front == rhs.front && lhs.back == rhs.back;
}

/**
 * @brief Check whether two regions are different.
 */
template <long n = 2>
bool operator!=(const Region<n>& lhs, const Region<n>& rhs) {
  return lhs.front != rhs.front || lhs.back != rhs.back;
}

/**
 * @brief Add a position.
 */
template <long n = 2>
Region<n>& operator+=(Region<n>& lhs, const Position<n>& rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @brief Subtract a position.
 */
template <long n = 2>
Region<n>& operator-=(Region<n>& lhs, const Position<n>& rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @brief Add a scalar to each coordinate.
 */
template <long n = 2>
Region<n>& operator+=(Region<n>& lhs, long rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @brief Subtract a scalar to each coordinate.
 */
template <long n = 2>
Region<n>& operator-=(Region<n>& lhs, long rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @brief Add 1 to each coordinate.
 */
template <long n = 2>
Region<n>& operator++(Region<n>& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @brief Subtract 1 to each coordinate.
 */
template <long n = 2>
Region<n>& operator--(Region<n>& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @brief Return the current region and then add 1 to each coordinate.
 */
template <long n = 2>
Region<n> operator++(Region<n>& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @brief Return the current region and then subtract 1 to each coordinate.
 */
template <long n = 2>
Region<n> operator--(Region<n>& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @brief Identity.
 */
template <long n = 2>
Region<n> operator+(const Region<n>& rhs) {
  return rhs;
}

/**
 * @brief Change the sign of each coordinate.
 */
template <long n = 2>
Region<n> operator-(const Region<n>& rhs) {
  rhs.front = -rhs.front;
  rhs.back = -rhs.back;
  return rhs;
}

/**
 * @brief Add a region and a positino.
 */
template <long n = 2>
Region<n> operator+(const Region<n>& lhs, const Position<n>& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract a region and a position.
 */
template <long n = 2>
Region<n> operator-(const Region<n>& lhs, const Position<n>& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @brief Add a region and a scalar.
 */
template <long n = 2>
Region<n> operator+(const Region<n>& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract a region and a scalar.
 */
template <long n = 2>
Region<n> operator-(const Region<n>& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/// @cond INTERNAL
// FIXME replace RegionScreener with PositionIterator?

/**
 * @ingroup image_data_classes
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
      m_region(region), m_current(region.front), m_fronts(followers), m_followers(followers) {}

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
  const Position<n>& next() {
    if (m_current == m_region.back) {
      m_current = m_region.front;
      m_followers = m_fronts;
      return m_current;
    }
    m_current[0]++;
    for (auto& f : m_followers) {
      f[0]++;
    }
    for (long i = 0; i < m_current.size(); ++i) {
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
  const Region<n>& m_region;
  Position<n> m_current;
  std::vector<Position<n>> m_fronts;
  std::vector<Position<n>> m_followers;
};

/// @endcond

} // namespace Fits
} // namespace Euclid

#endif
