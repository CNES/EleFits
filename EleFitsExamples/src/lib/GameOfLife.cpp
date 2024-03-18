// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsExamples/GameOfLife.h"

#include "Linx/Base/Random.h"

namespace Fits {

GameOfLife::GameOfLife(Linx::Index width, Linx::Index height, Linx::Index turns) :
    m_width(width), m_height(height), m_turns(turns), m_board({width, height, turns}), m_t(0),
    m_previous(m_board.section(0)), m_current(m_board.section(1))
{
  // The board is filled with zeros
}

Linx::Raster<Linx::Index> GameOfLife::generate(Linx::Index count, std::size_t seed)
{
  Linx::Raster<Linx::Index> lives({count, 2});
  auto x = lives.row(0).data();
  auto y = lives.row(1).data();
  Value* p = nullptr;

  Linx::UniformNoise<Linx::Index> x_generator(0, m_width - 1, seed);
  Linx::UniformNoise<Linx::Index> y_generator(0, m_height - 1, seed);

  for (Linx::Index i = 0; i < count; ++i, ++x, ++y) {
    do {
      *x = x_generator();
      *y = y_generator();
      p = &m_previous[{*x, *y}];
    } while (*p);
    *p = 1;
  }

  return lives;
}

const Linx::Raster<GameOfLife::Value, 3>& GameOfLife::run()
{
  // FIXME reset to enable rerunning
  while (m_t < m_turns) {
    update();
    next();
  }
  return m_board;
}

Linx::Index GameOfLife::next()
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
        m_current[p] = std::min(inc, std::numeric_limits<Value>::max());
      }
    } else { // Dead in previous frame
      if (lives == 3) {
        m_current[p] = 1;
      }
    }
  }
  return m_current;
}

Linx::Index GameOfLife::count_lives(const Linx::Position<2>& p) const
{
  static const Linx::Position<2> neighbors[] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  Linx::Index count = 0;
  for (const auto& n : neighbors) {
    const auto q = p + n;
    if (m_previous.domain().contains(q) && m_previous[q]) {
      ++count;
    }
  }
  return count;
}

} // namespace Fits
