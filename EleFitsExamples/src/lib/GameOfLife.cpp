// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsExamples/GameOfLife.h"

namespace Euclid {
namespace Fits {

GameOfLife::GameOfLife(long width, long height, long turns) :
    m_width(width), m_height(height), m_turns(turns), m_board({width, height, turns}), m_t(0),
    m_previous(m_board.section(0)), m_current(m_board.section(1))
{
  // The board is filled with zeros
}

const Linx::PtrRaster<GameOfLife::Value, 2>& GameOfLife::generate(long count)
{
  long done = 0;
  while (done <= count) {
    const long i = Test::generate_random_value<long>(0, m_width * m_height - 1);
    if (not m_previous[i]) {
      m_previous[i] = 1;
      ++done;
    }
  }
  return m_previous;
}

const Linx::Raster<GameOfLife::Value, 3>& GameOfLife::run()
{
  while (m_t < m_turns) {
    update();
    next();
  }
  return m_board;
}

long GameOfLife::next()
{
  m_previous = m_current;
  ++m_t;
  m_current = m_board.section(m_t);
  return m_t;
}

const Linx::PtrRaster<GameOfLife::Value, 2>& GameOfLife::update()
{
  for (const auto& p : m_previous.domain()) {
    const auto lives = count_lives(p);
    if (m_previous[p]) { // Live in previous frame
      if (lives < 2 || lives > 3) {
        m_current[p] = 0;
      } else {
        const Value inc = m_previous[p] + 1;
        m_current[p] = std::min(inc, std::numeric_limits<GameOfLife::Value>::max());
      }
    } else { // Dead in previous frame
      if (lives == 3) {
        m_current[p] = 1;
      }
    }
  }
  return m_current;
}

long GameOfLife::count_lives(const Linx::Position<2>& p) const
{
  const Linx::Position<2> neighbors[] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  long count = 0;
  for (const auto& n : neighbors) {
    const auto q = p + n;
    if (is_in_domain(q) && m_previous[q]) {
      ++count;
    }
  }
  return count;
}

bool GameOfLife::is_in_domain(const Linx::Position<2>& p) const
{
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