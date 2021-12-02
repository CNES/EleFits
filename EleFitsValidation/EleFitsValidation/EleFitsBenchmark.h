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

#ifndef _ELEFITS_VALIDATION_ELEFITSBENCHMARK_H
#define _ELEFITS_VALIDATION_ELEFITSBENCHMARK_H

#include "EleFits/MefFile.h"
#include "EleFitsValidation/Benchmark.h"

namespace Euclid {
namespace Fits {
namespace Validation {

/**
 * @brief EleFits without column I/O buffering.
 * @details
 * Tests on image HDUs are not supported (use `EleFitsBenchmark` instead).
 */
class EleFitsColwiseBenchmark : public Benchmark {

public:
  /**
   * @brief Destructor.
   */
  virtual ~EleFitsColwiseBenchmark() = default;

  /**
   * @brief Constructor.
   */
  explicit EleFitsColwiseBenchmark(const std::string& filename);

  /**
   * @brief Open file.
   */
  virtual void open() override;

  /**
   * @brief Close file.
   */
  virtual void close() override;

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
  /**
   * @brief The type and index of the i-th column.
   */
  template <long i>
  Indexed<typename std::tuple_element<i, BColumns>::type::Value> colIndexed() const;

protected:
  /**
   * @brief The MEF file handler.
   */
  MefFile m_f;
};

/**
 * @brief Standard EleFits.
 */
class EleFitsBenchmark : public EleFitsColwiseBenchmark {

public:
  /**
   * @brief Destructor.
   */
  virtual ~EleFitsBenchmark() = default;

  /**
   * @brief Constructor.
   */
  explicit EleFitsBenchmark(const std::string& filename);

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

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#define _ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL
#include "EleFitsValidation/impl/EleFitsBenchmark.hpp"
#undef _ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL

#endif
