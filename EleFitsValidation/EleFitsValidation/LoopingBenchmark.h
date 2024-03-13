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
  static constexpr Linx::Index Dimension = 3;

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
  LoopingBenchmark(Linx::Index side);

  /**
   * @brief Loop over positions built by looping over x, then y, and then z.
   */
  Duration loop_over_xyz();

  /**
   * @brief Loop over positions built by looping over z, then y, and then x.
   */
  Duration loop_over_zyx();

  /**
   * @brief Loop over positions via a position iterator.
   */
  Duration loop_over_positions();

  /**
   * @brief Loop over indices.
   */
  Duration loop_over_indices();

  /**
   * @brief Loop over values via a pixel iterator.
   */
  Duration loop_over_values();

private:

  const Linx::Index m_width;
  const Linx::Index m_height;
  const Linx::Index m_depth;
  const Test::RandomRaster<Value, Dimension> m_a;
  const Test::RandomRaster<Value, Dimension> m_b;
  Linx::Raster<Value, Dimension> m_c;
  Chronometer<Duration> m_chrono;
};

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
