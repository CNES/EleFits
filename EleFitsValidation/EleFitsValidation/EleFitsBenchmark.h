// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
   * @copybrief Benchmark::write_bintable
   * @details
   * Write the columns one-by-one to disable buffering.
   */
  virtual BChronometer::Unit write_bintable(const BColumns& columns) override;

  /**
   * @copybrief Benchmark::read_bintable
   * @details
   * Read the columns one-by-one to disable buffering.
   */
  virtual BColumns read_bintable(long index) override;

protected:
  /**
   * @brief The type and index of the i-th column.
   */
  template <long i>
  TypedKey<typename std::tuple_element<i, BColumns>::type::Value, long> col_indexed() const;

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
   * @copybrief Benchmark::write_image
   */
  virtual BChronometer::Unit write_image(const BRaster& raster) override;

  /**
   * @copybrief Benchmark::write_bintable
   */
  virtual BChronometer::Unit write_bintable(const BColumns& columns) override;

  /**
   * @copybrief Benchmark::read_image
   */
  virtual BRaster read_image(long index) override;

  /**
   * @copybrief Benchmark::read_bintable
   */
  virtual BColumns read_bintable(long index) override;
};

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#define _ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL
#include "EleFitsValidation/impl/EleFitsBenchmark.hpp"
#undef _ELEFITS_VALIDATION_ELEFITSBENCHMARK_IMPL

#endif
