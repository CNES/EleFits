// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_POSITIONITERATOR_H
#define _ELEFITSDATA_POSITIONITERATOR_H

#include "EleFitsData/Region.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_data_classes
 * @brief A helper class to screen a `Region`.
 * @details
 * Some current position is updated with increment operators.
 * The move is such that the corresponding offset in a `Raster` is always increasing.
 * In particular, when screening a whole `Raster`, the pixels are visited in the storage order.
 */
template <long N = 2>
class PositionIterator : public std::iterator<std::input_iterator_tag, Position<N>> {

public:
  /**
   * @brief Constructor.
   * @param region The region to be screened
   * @param followers Positions which follow the same moves as the current position
   */
  explicit PositionIterator(const Region<N>& region, const std::vector<Position<N>>& followers = {}) :
      m_region(region), m_current(region.front), m_fronts(followers), m_followers(followers) {}

  /**
   * @brief Dereference operator.
   */
  const Position<N>& operator*() const {
    return m_current;
  }

  /**
   * @brief Arrow operator.
   */
  const Position<N>* operator->() const {
    return &m_current;
  }

  /**
   * @brief Increment operator.
   */
  const Position<N>& operator++() {
    next();
    return m_current;
  }

  /**
   * @brief Increment operator.
   */
  const Position<N>* operator++(int) {
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
  const Position<N>& next() {
    if (m_current == m_region.back) {
      m_current[0]++;
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
  /**
   * @brief The screened region.
   */
  const Region<N>& m_region;

  /**
   * @brief The current position.
   */
  Position<N> m_current;

  /**
   * @brief The front positions of the followers.
   */
  std::vector<Position<N>> m_fronts;

  /**
   * @brief The current position of the followers.
   */
  std::vector<Position<N>> m_followers;
};

/**
 * @relates Region
 * @brief Iterator to the front position of a region.
 */
template <long N = 2>
PositionIterator<N> begin(const Region<N>& region) {
  return PositionIterator<N>(region);
}

/**
 * @relates Region
 * @brief Iterator to one past the back position of a region.
 */
template <long N = 2>
PositionIterator<N> end(const Region<N>& region) {
  Region<N> pastTheLast {region.back, region.back};
  pastTheLast.front[0]++;
  return PositionIterator<N>(pastTheLast);
}

} // namespace Fits
} // namespace Euclid

#endif
