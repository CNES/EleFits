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

#if defined(_ELEFITS_VALIDATION_CHRONOMETER_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsValidation/Chronometer.h"

#include <algorithm> // min_element, max_element
#include <cmath> // sqrt
#include <numeric> // inner_product

namespace Euclid {
namespace Fits {
namespace Validation {

template <typename TUnit>
Chronometer<TUnit>::Chronometer(TUnit offset) : m_tic(), m_toc(), m_running(false), m_incs(), m_elapsed(offset) {
  reset(offset);
}

template <typename TUnit>
void Chronometer<TUnit>::reset(TUnit offset) {
  m_toc = m_tic;
  m_running = false;
  m_incs.resize(0);
  m_elapsed = offset;
}

template <typename TUnit>
void Chronometer<TUnit>::start() {
  m_tic = std::chrono::steady_clock::now();
  m_running = true;
}

template <typename TUnit>
TUnit Chronometer<TUnit>::stop() {
  m_toc = std::chrono::steady_clock::now();
  m_running = false;
  const auto inc = std::chrono::duration_cast<TUnit>(m_toc - m_tic);
  m_elapsed += inc;
  m_incs.push_back(inc.count());
  return inc;
}

template <typename TUnit>
bool Chronometer<TUnit>::isRunning() const {
  return m_running; // TODO m_running could be removed by comparing m_toc to m_tic, but is it relevant?
}

template <typename TUnit>
TUnit Chronometer<TUnit>::last() const {
  return TUnit {typename TUnit::rep(m_incs[m_incs.size() - 1])};
}

template <typename TUnit>
TUnit Chronometer<TUnit>::elapsed() const {
  return m_elapsed;
}

template <typename TUnit>
std::size_t Chronometer<TUnit>::count() const {
  return m_incs.size();
}

template <typename TUnit>
const std::vector<double>& Chronometer<TUnit>::increments() const {
  return m_incs;
}

template <typename TUnit>
double Chronometer<TUnit>::mean() const {
  return std::accumulate(m_incs.begin(), m_incs.end(), 0.) / count();
}

template <typename TUnit>
double Chronometer<TUnit>::stdev() const {
  const auto m = mean();
  const auto s2 = std::inner_product(m_incs.begin(), m_incs.end(), m_incs.begin(), 0.);
  return std::sqrt(s2 / count() - m * m);
}

template <typename TUnit>
double Chronometer<TUnit>::min() const {
  return *std::min_element(m_incs.begin(), m_incs.end());
}

template <typename TUnit>
double Chronometer<TUnit>::max() const {
  return *std::max_element(m_incs.begin(), m_incs.end());
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
