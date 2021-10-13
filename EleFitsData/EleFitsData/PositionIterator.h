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

#ifndef _EL_FITSDATA_POSITIONITERATOR_H
#define _EL_FITSDATA_POSITIONITERATOR_H

#include "EleFitsData/Region.h"

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief A helper class to screen a `Region`.
 * @details
 * Some current position is updated with increment operators.
 * The move is such that the corresponding offset in a `Raster` is always increasing.
 * In particular, when screening a whole `Raster`, the pixels are visited in the storage order.
 */
template <long n = 2>
class PositionIterator : public std::iterator<std::input_iterator_tag, Position<n>> {

public:
  /**
   * @brief Constructor.
   * @param region The region to be screened
   * @param followers Positions which follow the same moves as the current position
   */
  explicit PositionIterator(const Region<n>& region, const std::vector<Position<n>>& followers = {}) :
      m_region(region), m_current(region.front), m_fronts(followers), m_followers(followers) {}

  /**
   * @brief Dereference operator.
   */
  const Position<n>& operator*() const {
    return m_current;
  }

  /**
   * @brief Arrow operator.
   */
  const Position<n>* operator->() const {
    return &m_current;
  }

  /**
   * @brief Increment operator.
   */
  const Position<n>& operator++() {
    next();
    return m_current;
  }

  /**
   * @brief Increment operator.
   */
  const Position<n>* operator++(int) {
    next();
    return &m_current;
  }

  /**
   * @brief Equality operator.
   */
  bool operator==(const PositionIterator& rhs) const {
    return m_current == rhs.m_current;
  }

  /**
   * @brief Non-equality operator.
   */
  bool operator!=(const PositionIterator& rhs) const {
    return m_current != rhs.m_current;
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

private:
  /**
   * @brief Update and get the current position and that of a follower.
   * @details
   * Move the current position by 1 pixel,
   * such that the corresponding index in a `Raster` would be increased to the next one.
   * 
   * Conventionally, `next(region.back)[i] = region.back[i]` for `i > 0`,
   * and `next(region.back)[0] = region.back[0] + 1`.
   */
  const Position<n>& next() {
    if (m_current == m_region.back) {
      m_current[0]++;
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
  /**
   * @brief The screened region.
   */
  const Region<n>& m_region;

  /**
   * @brief The current position.
   */
  Position<n> m_current;

  /**
   * @brief The front positions of the followers.
   */
  std::vector<Position<n>> m_fronts;

  /**
   * @brief The current position of the followers.
   */
  std::vector<Position<n>> m_followers;
};

template <long n = 2>
PositionIterator<n> begin(const Region<n>& region) {
  return PositionIterator<n>(region);
}

template <long n = 2>
PositionIterator<n> end(const Region<n>& region) {
  Region<n> pastTheLast { region.back, region.back };
  pastTheLast.front[0]++;
  return PositionIterator<n>(pastTheLast);
}

} // namespace FitsIO
} // namespace Euclid

#endif
