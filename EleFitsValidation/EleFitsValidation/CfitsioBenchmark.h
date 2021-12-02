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

#ifndef _ELEFITS_VALIDATION_CFITSIOBENCHMARK_H
#define _ELEFITS_VALIDATION_CFITSIOBENCHMARK_H

#include "EleCfitsioWrapper/CfitsioUtils.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsValidation/Benchmark.h"

#include <fitsio.h>

namespace Euclid {
namespace Fits {
namespace Validation {

/**
 * @brief Vanilla CFitsIO.
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
   * @param rowChunkSize The number of rows written at once if >0,
   * or -1 to write all rows at once,
   * or 0 for the buffer size.
   */
  CfitsioBenchmark(const std::string& filename, long rowChunkSize);

  /**
   * @brief Get the rowChunkSize parameter.
   */
  long rowChunkSize() const;

  /**
   * @brief Open file.
   */
  virtual void open() override;

  /**
   * @brief Close file.
   */
  virtual void close() override;

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

private:
  /**
   * @brief Get or compute the row chunk size.
   * @details
   * Depending on the value m_rowChunkSize:
   * - -1: All rows;
   * - >0: Use given value;
   * - 0: Compute size according to buffer size.
   */
  long computeRowChunkSize(long rowCount);

  /**
   * @brief Throw a CfitsioError if m_status != 0.
   */
  void mayThrow(const std::string& context) const;

  /**
   * @brief Setup the i-th column info (name, format, unit).
   */
  template <std::size_t i>
  void setupColumnInfo(
      const BColumns& columns,
      std::vector<std::string>& names,
      std::vector<std::string>& formats,
      std::vector<std::string>& units);

  /**
   * @brief Write a chunk of the i-th column.
   */
  template <std::size_t i>
  void writeColumn(const BColumns& columns, long firstRow, long rowCount);

  /**
   * @brief Read the size and metadata of the i-th column.
   */
  template <std::size_t i>
  void initColumn(BColumns& columns, long rowCount);

  /**
   * @brief Read a chunk of the i-th column.
   */
  template <std::size_t i>
  void readColumn(BColumns& columns, long firstRow, long rowCount);

private:
  /** @brief The Fits file. */
  fitsfile* m_fptr;
  /** @brief The CFitsIO status code. */
  int m_status;
  /** @brief The number of rows to write at once. */
  long m_rowChunkSize;
};

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#define _ELEFITS_VALIDATION_CFITSIOBENCHMARK_IMPL
#include "EleFitsValidation/impl/CfitsioBenchmark.hpp"
#undef _ELEFITS_VALIDATION_CFITSIOBENCHMARK_IMPL

#endif
