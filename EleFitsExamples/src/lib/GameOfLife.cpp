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

#include "EleFitsExamples/GameOfLife.h"

namespace Euclid {
namespace Fits {

GameOfLife::GameOfLife(long width, long height, long turns) :
    m_width(width), m_height(height), m_turns(turns), m_board({width, height, turns}), m_t(0),
    m_previous(m_board.section(0)), m_current(m_board.section(1)) {
  // The board is filled with zeros
}

const Raster<GameOfLife::Value, 2>& GameOfLife::generate(long count) {
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

const Raster<GameOfLife::Value, 3>& GameOfLife::run() {
  while (m_t < m_turns) {
    update();
    next();
  }
  return m_board;
}

long GameOfLife::next() {
  m_previous = m_current;
  ++m_t;
  m_current = m_board.section(m_t);
  return m_t;
}

const Raster<GameOfLife::Value, 2>& GameOfLife::update() {
  for (const auto& p : m_previous.domain()) {
    const auto lifes = countLifes(p);
    if (m_previous[p]) { // Live in previous frame
      if (lifes < 2 || lifes > 3) {
        m_current[p] = 0;
      } else {
        const Value inc = m_previous[p] + 1;
        m_current[p] = std::min(inc, std::numeric_limits<GameOfLife::Value>::max());
      }
    } else { // Dead in previous frame
      if (lifes == 3) {
        m_current[p] = 1;
      }
    }
  }
  return m_current;
}

long GameOfLife::countLifes(const Position<2>& p) const {
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

bool GameOfLife::isInDomain(const Position<2>& p) const {
  if (p[0] < 0 || p[0] >= m_width) {
    return false;
  }
  if (p[1] < 0 || p[1] >= m_height) {
    return false;
  }
  return true;
}

} // namespace Fits
} // namespace Euclid