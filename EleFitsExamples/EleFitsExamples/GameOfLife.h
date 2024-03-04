// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
  GameOfLife(long width, long height, long turns);

  /**
   * @brief Generate lives at random positions.
   * @param count The number of lives to generate (should be much smaller than the number of cells)
   */
  const Linx::PtrRaster<Value, 2>& generate(long count);

  /**
   * @brief Run the game.
   */
  const Linx::Raster<Value, 3>& run();

  /**
   * @brief Move to the next frame.
   */
  long next();

  /**
   * @brief Update the current frame.
   */
  const PtrRaster<Value, 2>& update();

  /**
   * @brief Count lives around a given position.
   */
  long count_lives(const Position<2>& p) const;

  /**
   * @brief Check whether a given position is in the board domain.
   */
  bool is_in_domain(const Position<2>& p) const;

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
  Linx::Raster<Value, 3> m_board;

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
