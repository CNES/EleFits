// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
