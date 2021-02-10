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

#ifndef _EL_FITSIO_VALIDATION_CFITSIOBENCHMARK_H
#define _EL_FITSIO_VALIDATION_CFITSIOBENCHMARK_H

#include <fitsio.h>

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

#include "EL_FitsIO_Validation/Benchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief Vanilla CFitsIO.
 */
class CfitsioBenchmark : public Benchmark {
public:
  /**
   * @brief Destructor.
   */
  virtual ~CfitsioBenchmark() = default;

  /**
   * @brief Constructor.
   */
  CfitsioBenchmark(const std::string& filename);

  /**
   * @copybrief Benchmark::writeImage
   */
  virtual BChronometer::Unit writeImage(const BRaster& raster) override;

  /**
   * @copybrief Benchmark::writeBintable
   */
  virtual BChronometer::Unit writeBintable(const BColumns& columns) override;

private:
  /**
   * @brief Setup the i-th column info (name, format, unit).
   */
  template <std::size_t i>
  void setupColumnInfo(
      const BColumns& columns,
      std::vector<std::string>& names,
      std::vector<std::string>& formats,
      std::vector<std::string> units);

  /**
   * @brief Write the i-th column.
   */
  template <std::size_t i>
  void writeColumn(const BColumns& columns, long rowCount);

private:
  /** @brief The Fits file. */
  fitsfile* m_fptr;
  /** @brief The CFitsIO status code. */
  int m_status;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#define _EL_FITSIO_VALIDATION_CFITSIOBENCHMARK_IMPL
#include "EL_FitsIO_Validation/impl/CfitsioBenchmark.hpp"
#undef _EL_FITSIO_VALIDATION_CFITSIOBENCHMARK_IMPL

#endif
