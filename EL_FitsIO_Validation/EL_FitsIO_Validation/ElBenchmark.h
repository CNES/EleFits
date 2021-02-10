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
class ElUnbufferedBenchmark : public Benchmark {
public:
  /**
   * @brief Destructor.
   */
  virtual ~ElUnbufferedBenchmark() = default;

  /**
   * @brief Constructor.
   */
  ElUnbufferedBenchmark(const std::string& filename);

  /**
   * @copybrief Benchmark::writeBintable
   * @details
   * Write the columns one-by-one to disable buffering.
   */
  virtual BChronometer::Unit writeBintable(const BColumns& columns) override;

protected:
  /** @brief The MEF file handler. */
  MefFile m_f;
};

/**
 * @brief Standard EL_FitsIO.
 */
class ElBenchmark : public ElUnbufferedBenchmark {
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
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
