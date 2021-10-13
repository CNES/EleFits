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

#ifndef _ELEFITS_VALIDATION_CHRONOMETER_H
#define _ELEFITS_VALIDATION_CHRONOMETER_H

#include <chrono>
#include <cmath> // sqrt
#include <numeric> // inner_product
#include <vector>

namespace Euclid {
namespace Fits {
namespace Test {

/**
 * @brief A simple chronometer with increment times and elapsed time caching.
 * @tparam TUnit The time unit, e.g. `std::chrono::milliseconds`
 * @details
 * Each time the chronometer is started and stoped, an increment is computed,
 * and the total elapsed time is incremented.
 * An offset can be provided, which is the initial value of the elapsed time,
 * but has no effect on the increments.
 *
 * Simple statistics on the increments can be computed (e.g. mean increment).
 *
 * The chronometer can be reset, which means that the list of increments is emptied,
 * and the elapsed time is set to 0 or the offset.
 */
template <typename TUnit>
class Chronometer {
public:
  using Unit = TUnit;

  /**
   * @brief Create a chronometer with optional offset.
   */
  Chronometer(TUnit offset = TUnit());

  /**
   * @brief Reset the chronometer with optional offset.
   */
  void reset(TUnit offset = TUnit());

  /**
   * @brief Start or restart the chronometer.
   */
  void start();

  /**
   * @brief Stop the chronometer and get the last time increment.
   */
  TUnit stop();

  /**
   * @brief Test whether the chronometer is running.
   */
  bool isRunning() const;

  /**
   * @brief The last increment.
   */
  TUnit last() const;

  /**
   * @brief The elapsed time.
   */
  TUnit elapsed() const;

  /**
   * @brief The number of increments.
   */
  std::size_t count() const;

  /**
   * @brief Get the increments.
   */
  const std::vector<double>& increments() const;

  /**
   * @brief The mean of the increments.
   */
  double mean() const;

  /**
   * @brief The standard deviation of the increments.
   */
  double stdev() const;

  /**
   * @brief The minimum increment.
   */
  double min() const;

  /**
   * @brief The maximum increment.
   */
  double max() const;

private:
  /**
   * @brief The time at which start() was called.
   */
  std::chrono::steady_clock::time_point m_tic;

  /**
   * @brief The time at which stop() was called.
   */
  std::chrono::steady_clock::time_point m_toc;

  /**
   * @brief Flag the chronometer as running (started and not stopped).
   */
  bool m_running;

  /**
   * @brief The list of increments.
   */
  std::vector<double> m_incs;

  /**
   * @brief The total m_elapsed time.
   */
  TUnit m_elapsed;
};

} // namespace Test
} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_VALIDATION_CHRONOMETER_IMPL
#include "EleFitsValidation/impl/Chronometer.hpp"
#undef _ELEFITS_VALIDATION_CHRONOMETER_IMPL
/// @endcond

#endif
