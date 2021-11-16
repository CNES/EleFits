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

#ifndef _ELEFITSEXAMPLES_GAMEOFLIFE_H
#define _ELEFITSEXAMPLES_GAMEOFLIFE_H

#include "EleFitsData/Raster.h"
#include "EleFitsData/TestUtils.h"

namespace Euclid {
namespace Fits {

/**
 * @brief A Game of Life as an example usage of `Raster` and a few features like slicing.
 */
class GameOfLife {

public:
  /**
   * @brief The cell type.
   */
  using Value = unsigned char;

  /**
   * @brief Constructor.
   */
  GameOfLife(long width, long height, long turns) :
      m_width(width), m_height(height), m_turns(turns), m_board({width, height, turns}), m_t(0),
      m_previous(m_board.section(0)), m_current(m_board.section(1)) {
    // The board is filled with zeros
  }

  /**
   * @brief Generate lifes at random positions.
   * @param count The number of lifes to generate (should be much smaller than the number of cells)
   */
  const Raster<Value, 2>& generate(long count) {
    long done = 0;
    while (done <= count) {
      const long i = Test::generateRandomValue<long>(0, m_width * m_height - 1);
      if (not *(m_previous.data() + i)) {
        *(m_previous.data() + i) = 1;
        ++done;
      }
    }
    return m_previous;
  }

  /**
   * @brief Run the game.
   */
  const Raster<Value, 3>& run() {
    while (m_t < m_turns) {
      update();
      next();
    }
    return m_board;
  }

  /**
   * @brief Move to the next frame.
   */
  long next() {
    m_previous = m_current;
    ++m_t;
    m_current = m_board.section(m_t);
    return m_t;
  }

  /**
   * @brief Update the current frame.
   */
  const Raster<Value, 2>& update() {
    for (const auto& p : m_previous.domain()) {
      const auto lifes = countLifes(p);
      if (m_previous[p]) { // Live in previous frame
        if (lifes < 2 || lifes > 3) {
          m_current[p] = 0;
        } else {
          m_current[p] = lifes;
        }
      } else { // Dead in previous frame
        if (lifes == 3) {
          m_current[p] = 1;
        }
      }
    }
    return m_current;
  }

  /**
   * @brief Count lifes around a given position.
   */
  long countLifes(const Position<2>& p) const {
    const Position<2> neighbors[] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    long count = 0;
    for (const auto& n : neighbors) {
      const auto q = p + n;
      if (isInDomain(q) && m_previous[q]) {
        ++count;
      }
    }
    return count;
  }

  /**
   * @brief Check whether a given position is in the board domain.
   */
  bool isInDomain(const Position<2>& p) const {
    if (p[0] < 0 || p[0] >= m_width) {
      return false;
    }
    if (p[1] < 0 || p[1] >= m_height) {
      return false;
    }
    return true;
  }

private:
  /**
   * @brief The board width.
   */
  long m_width;

  /**
   * @brief The board height.
   */
  long m_height;

  /**
   * @brief The number of turns.
   */
  long m_turns;

  /**
   * @brief The board and its evolution in time.
   */
  VecRaster<Value, 3> m_board;

  /**
   * @brief The current turn index (aka time).
   */
  long m_t;

  /**
   * @brief The previous frame.
   */
  PtrRaster<Value, 2> m_previous;

  /**
   * @brief The current frame.
   */
  PtrRaster<Value, 2> m_current;
};

} // namespace Fits
} // namespace Euclid

#endif
