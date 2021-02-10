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
  Chronometer(TUnit offset = TUnit()) : m_tic(), m_toc(), m_incs(), m_elapsed(offset) {
    reset();
  }

  /**
   * @brief Reset the chronometer (elapsed = 0).
   */
  void reset() {
    m_toc = m_tic;
    m_incs.empty();
    m_elapsed = TUnit();
  }

  /**
   * @brief Start or restart the chronometer.
   */
  void start() {
    m_tic = std::chrono::steady_clock::now();
  }

  /**
   * @brief Stop the chronometer and get the last time increment.
   */
  TUnit stop() {
    m_toc = std::chrono::steady_clock::now();
    const auto inc = std::chrono::duration_cast<TUnit>(m_toc - m_tic);
    m_elapsed += inc;
    m_incs.push_back(inc.count());
    return inc;
  }

  /**
   * @brief The last increment.
   */
  TUnit lastIncrement() const {
    return m_incs[m_incs.size() - 1];
  }

  /**
   * @brief The elapsed time.
   */
  TUnit elapsed() const {
    return m_elapsed;
  }

  /**
   * @brief The number of increments.
   */
  std::size_t count() const {
    return m_incs.size();
  }

  /**
   * @brief The mean of the increments.
   */
  double mean() const {
    return std::accumulate(m_incs.begin(), m_incs.end(), 0.) / count();
  }

  /**
   * @brief The standard deviation of the increments.
   */
  double stdev() const {
    const auto m = mean();
    const auto s2 = std::inner_product(m_incs.begin(), m_incs.end(), m_incs.begin(), 0.);
    return std::sqrt(s2 / count() - m * m);
  }

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

#endif
