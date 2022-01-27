/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
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
  GameOfLife(long width, long height, long turns);

  /**
   * @brief Generate lifes at random positions.
   * @param count The number of lifes to generate (should be much smaller than the number of cells)
   */
  const PtrRaster<Value, 2>& generate(long count);

  /**
   * @brief Run the game.
   */
  const VecRaster<Value, 3>& run();

  /**
   * @brief Move to the next frame.
   */
  long next();

  /**
   * @brief Update the current frame.
   */
  const PtrRaster<Value, 2>& update();

  /**
   * @brief Count lifes around a given position.
   */
  long countLifes(const Position<2>& p) const;

  /**
   * @brief Check whether a given position is in the board domain.
   */
  bool isInDomain(const Position<2>& p) const;

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
