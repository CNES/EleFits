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

#ifndef _EL_FITSIO_VALIDATION_BENCHMARK_H
#define _EL_FITSIO_VALIDATION_BENCHMARK_H

#include <tuple>

#include "ElementsKernel/Logging.h"

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/Column.h"

#include "EL_FitsIO_Validation/Chronometer.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief The raster type used for benchmarking.
 */
using BRaster = VecRaster<std::int64_t, 1>;

/**
 * @brief The column types used for benchmarking.
 */
using BColumns = std::tuple<
    const VecColumn<unsigned char>&,
    const VecColumn<std::int32_t>&,
    const VecColumn<std::int64_t>&,
    const VecColumn<float>&,
    const VecColumn<double>&,
    const VecColumn<std::complex<float>>&,
    const VecColumn<std::complex<double>>&,
    // const VecColumn<std::string>&, //TODO to be handled specially
    const VecColumn<char>&,
    const VecColumn<std::uint32_t>&,
    const VecColumn<std::uint64_t>&>;

/**
 * @brief The number of columns.
 */
constexpr std::size_t columnCount = std::tuple_size<BColumns>::value;

/**
 * @brief The chronometer used for benchmarking.
 */
using BChronometer = Chronometer<std::chrono::milliseconds>;

/**
 * @brief The exception which is thrown when a test case is not implemented.
 */
struct TestCaseNotImplemented : public std::exception {
  TestCaseNotImplemented(const std::string& testCaseName) : message("Test case not implemented: " + testCaseName) {
  }
  virtual const char* what() const noexcept override {
    return message.c_str();
  }
  std::string message;
};

/**
 * @brief The base class for all test cases of the benchmark.
 */
class Benchmark {
public:
  virtual ~Benchmark() = default;

  Benchmark() : m_chrono(), m_logger(Elements::Logging::getLogger("Benchmark")) {
  }

  const BChronometer& writeImages(int count, const BRaster& raster) {
    m_chrono.reset();
    for (int i = 0; i < count; ++i) {
      const auto inc = writeImage(raster);
      m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
    }
    const auto total = m_chrono.elapsed();
    m_logger.debug() << "TOTAL: " << total.count() << "ms";
    return m_chrono;
  }

  const BChronometer& writeBintables(int count, const BColumns& columns) { // TODO avoid duplication
    m_chrono.reset();
    for (int i = 0; i < count; ++i) {
      const auto inc = writeBintable(columns);
      m_logger.debug() << i + 1 << "/" << count << ": " << inc.count() << "ms";
    }
    const auto total = m_chrono.elapsed();
    m_logger.debug() << "TOTAL: " << total.count() << "ms";
    return m_chrono;
  }

  virtual BChronometer::Unit writeImage(const BRaster& raster) {
    throw TestCaseNotImplemented("Write image");
  }

  virtual BChronometer::Unit writeBintable(const BColumns& columns) {
    throw TestCaseNotImplemented("Write bintable");
  }

protected:
  BChronometer m_chrono;
  Elements::Logging m_logger;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
