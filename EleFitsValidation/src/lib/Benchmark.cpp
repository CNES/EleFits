// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/Benchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

Benchmark::Benchmark(const std::string& filename) :
    m_filename(filename), m_chrono(), m_logger(Elements::Logging::getLogger("Benchmark")) {}

const BChronometer& Benchmark::write_images(long count, const BRaster& raster) {
  open();
  m_chrono.reset();
  m_logger.debug() << "First pixel: " << raster.at({0});
  m_logger.debug() << "Last pixel: " << raster.at({-1});
  for (long i = 0; i < count; ++i) {
    const auto inc = write_image(raster);
    m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  close();
  return m_chrono;
}

const BChronometer& Benchmark::write_bintables(long count, const BColumns& columns) { // TODO avoid duplication
  open();
  m_chrono.reset();
  m_logger.debug() << "First column, first row: " << std::get<0>(columns).at(0, 0);
  m_logger.debug() << "Last column, last row: " << std::get<ColumnCount - 1>(columns).at(-1, -1);
  for (long i = 0; i < count; ++i) {
    const auto inc = write_bintable(columns);
    m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  close();
  return m_chrono;
}

const BChronometer& Benchmark::read_images(long first, long count) {
  open();
  m_chrono.reset();
  for (long i = 0; i < count; ++i) {
    const auto raster = read_image(first + i);
    m_logger.debug() << i + 1 << "/" << count << ": " << m_chrono.last().count() << "ms";
    m_logger.debug() << "\tFirst pixel: " << raster.at({0});
    m_logger.debug() << "\tLast pixel: " << raster.at({-1});
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  close();
  return m_chrono;
}

const BChronometer& Benchmark::read_bintables(long first, long count) {
  open();
  m_chrono.reset();
  for (long i = 0; i < count; ++i) {
    const auto columns = read_bintable(i + first);
    m_logger.debug() << i + 1 << "/" << count << ": " << m_chrono.last().count() << "ms";
    m_logger.debug() << "\tFirst column, first row: " << std::get<0>(columns).at(0, 0);
    m_logger.debug() << "\tLast column, last row: " << std::get<ColumnCount - 1>(columns).at(-1, -1);
  }
  const auto total = m_chrono.elapsed();
  m_logger.debug() << "TOTAL: " << total.count() << "ms";
  close();
  return m_chrono;
}

void BenchmarkFactory::register_benchmark_maker(const std::string& key, BenchmarkMaker factory) {
  if (m_register.find(key) != m_register.end()) {
    throw std::runtime_error(std::string("Benchmark already registered: ") + key);
  }
  m_register[key] = factory;
}

std::unique_ptr<Benchmark>
BenchmarkFactory::create_benchmark(const std::string& key, const std::string& filename) const {
  const auto it = m_register.find(key);
  if (it == m_register.end()) {
    throw TestCaseNotImplemented(key);
  }
  return it->second(filename);
}

std::vector<std::string> BenchmarkFactory::keys() const {
  std::vector<std::string> res;
  for (const auto& kv : m_register) {
    res.push_back(kv.first);
  }
  return res;
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid
