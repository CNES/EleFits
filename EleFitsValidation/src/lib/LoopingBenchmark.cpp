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

#include "EleFitsValidation/LoopingBenchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

LoopingBenchmark::LoopingBenchmark(long side) :
    m_width(side), m_height(side), m_depth(side), m_a({side, side, side}), m_b({side, side, side}),
    m_c({side, side, side}) {}

BChronometer::Unit LoopingBenchmark::loopOverXyz() {
  m_chrono.start();
  for (long x = 0; x < m_width; ++x) {
    for (long y = 0; y < m_height; ++y) {
      for (long z = 0; z < m_depth; ++z) {
        m_c[{x, y, z}] = m_a[{x, y, z}] + m_b[{x, y, z}];
      }
    }
  }
  return m_chrono.stop();
}

BChronometer::Unit LoopingBenchmark::loopOverZyx() {
  m_chrono.start();
  for (long z = 0; z < m_depth; ++z) {
    for (long y = 0; y < m_height; ++y) {
      for (long x = 0; x < m_width; ++x) {
        m_c[{x, y, z}] = m_a[{x, y, z}] + m_b[{x, y, z}];
      }
    }
  }
  return m_chrono.stop();
}

BChronometer::Unit LoopingBenchmark::loopOverPositions() {
  m_chrono.start();
  for (const auto& p : m_c.domain()) {
    m_c[p] = m_a[p] + m_b[p];
  }
  return m_chrono.stop();
}

BChronometer::Unit LoopingBenchmark::loopOverIndices() {
  m_chrono.start();
  const auto size = m_c.size();
  for (long i = 0; i < size; ++i) {
    m_c[i] = m_a[i] + m_b[i];
  }
  return m_chrono.stop();
}

BChronometer::Unit LoopingBenchmark::loopOverValues() {
  m_chrono.start();
  auto ait = m_a.data(); // FIXME Raster::begin(), end()
  auto bit = m_b.data();
  const auto begin = m_c.data();
  const auto end = begin + m_c.size();
  for (auto cit = begin; cit != end; ++cit, ++ait, ++bit) {
    *cit = *ait + *bit;
  }
  return m_chrono.stop();
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid