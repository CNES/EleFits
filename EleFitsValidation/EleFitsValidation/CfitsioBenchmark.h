// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_VALIDATION_CFITSIOBENCHMARK_H
#define _ELEFITS_VALIDATION_CFITSIOBENCHMARK_H

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsUtils/StringUtils.h"
#include "EleFitsValidation/Benchmark.h"

#include <fitsio.h>

namespace Fits {
namespace Validation {

/**
 * @brief Vanilla CFITSIO.
 */
class CfitsioBenchmark : public Benchmark {
public:

  /**
   * @brief Destructor.
   */
  virtual ~CfitsioBenchmark();

  /**
   * @brief Constructor.
   * @param filename The file to be written
   * @param chunk_row_count The number of rows written at once if >0,
   * or -1 to write all rows at once,
   * or 0 for the buffer size.
   */
  CfitsioBenchmark(const std::string& filename, Linx::Index chunk_row_count);

  /**
   * @brief Get the chunk_row_count parameter.
   */
  Linx::Index chunk_row_count() const;

  /**
   * @brief Open file.
   */
  virtual void open() override;

  /**
   * @brief Close file.
   */
  virtual void close() override;

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
  virtual BRaster read_image(Linx::Index index) override;

  /**
   * @copybrief Benchmark::read_bintable
   */
  virtual BColumns read_bintable(Linx::Index index) override;

private:

  /**
   * @brief Get or compute the row chunk size.
   * @details
   * Depending on the value m_chunk_row_count:
   * - -1: All rows;
   * - >0: Use given value;
   * - 0: Compute size according to buffer size.
   */
  Linx::Index compute_chunk_row_count(Linx::Index row_count);

  /**
   * @brief Throw a CfitsioError if m_status != 0.
   */
  void may_throw(const std::string& context) const;

  /**
   * @brief Setup the i-th column info (name, format, unit).
   */
  template <std::size_t i>
  void setup_column_info(
      const BColumns& columns,
      std::vector<std::string>& names,
      std::vector<std::string>& formats,
      std::vector<std::string>& units);

  /**
   * @brief Write a chunk of the i-th column.
   */
  template <std::size_t i>
  void write_column(const BColumns& columns, Linx::Index first_row, Linx::Index row_count);

  /**
   * @brief Read the size and metadata of the i-th column.
   */
  template <std::size_t i>
  void init_column(BColumns& columns, Linx::Index row_count);

  /**
   * @brief Read a chunk of the i-th column.
   */
  template <std::size_t i>
  void read_column(BColumns& columns, Linx::Index first_row, Linx::Index row_count);

private:

  /** @brief The FITS file. */
  fitsfile* m_fptr;
  /** @brief The CFITSIO status code. */
  int m_status;
  /** @brief The number of rows to write at once. */
  Linx::Index m_chunk_row_count;
};

} // namespace Validation
} // namespace Fits

#define _ELEFITS_VALIDATION_CFITSIOBENCHMARK_IMPL
#include "EleFitsValidation/impl/CfitsioBenchmark.hpp"
#undef _ELEFITS_VALIDATION_CFITSIOBENCHMARK_IMPL

#endif
