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
  virtual ~CfitsioBenchmark() = default;

  CfitsioBenchmark(const std::string& filename);

  virtual BChronometer::Unit writeImage(const BRaster& raster) override;

  virtual BChronometer::Unit writeBintable(const BColumns& columns) override;

private:
  template <std::size_t i>
  void setupColumnInfo(
      const BColumns& columns,
      std::vector<std::string>& names,
      std::vector<std::string>& formats,
      std::vector<std::string> units) {
    const auto& col = std::get<i>(columns);
    names[i] = col.info.name;
    formats[i] = Cfitsio::TypeCode<typename std::decay_t<decltype(col)>::Value>::tform(col.info.repeatCount);
    units[i] = col.info.unit;
  }

  template <std::size_t i>
  void writeColumn(const BColumns& columns, long rowCount) {
    const auto& col = std::get<i>(columns);
    auto nonconstVec = col.vector();
    fits_write_col(
        m_fptr,
        Cfitsio::TypeCode<typename std::decay_t<decltype(col)>::Value>::forBintable(),
        i + 1,
        1,
        1,
        rowCount,
        nonconstVec.data(),
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
  }

private:
  fitsfile* m_fptr;
  int m_status;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#endif
