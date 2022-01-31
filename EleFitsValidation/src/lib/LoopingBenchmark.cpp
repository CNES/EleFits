// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/LoopingBenchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

LoopingBenchmark::LoopingBenchmark(long side) :
    m_width(side), m_height(side), m_depth(side), m_a({side, side, side}), m_b({side, side, side}),
    m_c({side, side, side}) {}

LoopingBenchmark::Duration LoopingBenchmark::loopOverXyz() {
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

LoopingBenchmark::Duration LoopingBenchmark::loopOverZyx() {
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

LoopingBenchmark::Duration LoopingBenchmark::loopOverPositions() {
  m_chrono.start();
  for (const auto& p : m_c.domain()) {
    m_c[p] = m_a[p] + m_b[p];
  }
  return m_chrono.stop();
}

LoopingBenchmark::Duration LoopingBenchmark::loopOverIndices() {
  m_chrono.start();
  const auto size = m_c.size();
  for (std::size_t i = 0; i < size; ++i) {
    m_c[i] = m_a[i] + m_b[i];
  }
  return m_chrono.stop();
}

LoopingBenchmark::Duration LoopingBenchmark::loopOverValues() {
  m_chrono.start();
  auto ait = m_a.begin();
  auto bit = m_b.begin();
  const auto begin = m_c.begin();
  const auto end = m_c.end();
  for (auto cit = begin; cit != end; ++cit, ++ait, ++bit) {
    *cit = *ait + *bit;
  }
  return m_chrono.stop();
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid