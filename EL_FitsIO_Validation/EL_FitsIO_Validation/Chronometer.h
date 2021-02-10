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

#ifndef _EL_FITSIO_VALIDATION_CHRONOMETER_H
#define _EL_FITSIO_VALIDATION_CHRONOMETER_H

#include <chrono>
#include <cmath> // sqrt
#include <numeric> // inner_product
#include <vector>

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief A simple chronometer with m_elapsed time caching.
 * @tparam TUnit The time unit, e.g. `std::chrono::milliseconds`
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
   * @brief Reset the chronometer (elapsed = 0).
   */
  void reset();

  /**
   * @brief Start or restart the chronometer.
   */
  void start();

  /**
   * @brief Stop the chronometer and get the last time increment.
   */
  TUnit stop();

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
   * @brief The mean of the increments.
   */
  double mean() const;

  /**
   * @brief The standard deviation of the increments.
   */
  double stdev() const;

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
   * @brief The list of increments.
   */
  std::vector<double> m_incs;

  /**
   * @brief The total m_elapsed time.
   */
  TUnit m_elapsed;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSIO_VALIDATION_CHRONOMETER_IMPL
#include "EL_FitsIO_Validation/impl/Chronometer.hpp"
#undef _EL_FITSIO_VALIDATION_CHRONOMETER_IMPL
/// @endcond

#endif
