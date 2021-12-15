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

#ifndef _ELEFITSVALIDATION_LOOPINGBENCHMARK_H
#define _ELEFITSVALIDATION_LOOPINGBENCHMARK_H

#include "EleFitsData/TestRaster.h"
#include "EleFitsValidation/Chronometer.h"

namespace Euclid {
namespace Fits {
namespace Validation {

/**
 * @brief Benchmark to measure the impact of pixel looping approach.
 */
class LoopingBenchmark {

public:
  /**
   * @brief The raster dimension.
   */
  static constexpr long Dim = 3;

  /**
   * @brief The raster value type.
   */
  using Value = char;

  /**
   * @brief The duration unit.
   */
  using Duration = std::chrono::milliseconds;

  /**
   * @brief Constructor.
   */
  LoopingBenchmark(long side);

  /**
   * @brief Loop over positions built by looping over x, then y, and then z.
   */
  Duration loopOverXyz();

  /**
   * @brief Loop over positions built by looping over z, then y, and then x.
   */
  Duration loopOverZyx();

  /**
   * @brief Loop over positions via a position iterator.
   */
  Duration loopOverPositions();

  /**
   * @brief Loop over indices.
   */
  Duration loopOverIndices();

  /**
   * @brief Loop over values via a pixel iterator.
   */
  Duration loopOverValues();

private:
  const long m_width;
  const long m_height;
  const long m_depth;
  const Test::RandomRaster<Value, Dim> m_a;
  const Test::RandomRaster<Value, Dim> m_b;
  VecRaster<Value, Dim> m_c;
  Chronometer<Duration> m_chrono;
};

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
