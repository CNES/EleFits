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

#ifndef _EL_FITSIO_VALIDATION_ELBENCHMARK_H
#define _EL_FITSIO_VALIDATION_ELBENCHMARK_H

#include "EL_FitsFile/MefFile.h"

#include "EL_FitsIO_Validation/Benchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief EL_FitsIO without column I/O buffering.
 * @details
 * Tests on image HDUs are not supported (use ElBenchmark instead).
 */
class ElColwiseBenchmark : public Benchmark {
public:
  /**
   * @brief Destructor.
   */
  virtual ~ElColwiseBenchmark() = default;

  /**
   * @brief Constructor.
   */
  ElColwiseBenchmark(const std::string& filename);

  /**
   * @copybrief Benchmark::writeBintable
   * @details
   * Write the columns one-by-one to disable buffering.
   */
  virtual BChronometer::Unit writeBintable(const BColumns& columns) override;

  /**
   * @copybrief Benchmark::readBintable
   * @details
   * Read the columns one-by-one to disable buffering.
   */
  virtual BColumns readBintable(long index) override;

protected:
  template <long i>
  Indexed<typename std::tuple_element<i, BColumns>::type::Value> colIndexed() const;

protected:
  /** @brief The MEF file handler. */
  MefFile m_f;
};

/**
 * @brief Standard EL_FitsIO.
 */
class ElBenchmark : public ElColwiseBenchmark {
public:
  /**
   * @brief Destructor.
   */
  virtual ~ElBenchmark() = default;

  /**
   * @brief Constructor.
   */
  ElBenchmark(const std::string& filename);

  /**
   * @copybrief Benchmark::writeImage
   */
  virtual BChronometer::Unit writeImage(const BRaster& raster) override;

  /**
   * @copybrief Benchmark::writeBintable
   */
  virtual BChronometer::Unit writeBintable(const BColumns& columns) override;

  /**
   * @copybrief Benchmark::readImage
   */
  virtual BRaster readImage(long index) override;

  /**
   * @copybrief Benchmark::readBintable
   */
  virtual BColumns readBintable(long index) override;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#define _EL_FITSIO_VALIDATION_ELBENCHMARK_IMPL
#include "EL_FitsIO_Validation/impl/ElBenchmark.hpp"
#undef _EL_FITSIO_VALIDATION_ELBENCHMARK_IMPL

#endif
