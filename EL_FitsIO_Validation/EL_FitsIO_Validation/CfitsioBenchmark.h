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

  CfitsioBenchmark(const std::string& filename) : Benchmark(), m_fptr(nullptr), m_status(0) {
    fits_create_file(&m_fptr, (std::string("!") + filename).c_str(), &m_status);
  }

  virtual BChronometer::Unit writeImage(const BRaster& raster) override {
    m_chrono.start();
    auto nonconstShape = raster.shape;
    fits_create_img(
        m_fptr,
        Cfitsio::TypeCode<BRaster::Value>::bitpix(),
        raster.shape.size(),
        nonconstShape.data(),
        &m_status);
    std::vector<BRaster::Value> nonconstData(raster.data(), raster.data() + raster.size());
    fits_write_img(
        m_fptr,
        Cfitsio::TypeCode<BRaster::Value>::forImage(),
        1,
        raster.size(),
        nonconstData.data(),
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
    return m_chrono.stop();
  }

  virtual BChronometer::Unit writeBintable(const BColumns& columns) override {
    long rowCount = std::get<0>(columns).rowCount();
    std::vector<std::string> names(rowCount);
    std::vector<std::string> formats(rowCount);
    std::vector<std::string> units(rowCount);
    setupColumnInfo<0>(columns, names, formats, units);
    setupColumnInfo<1>(columns, names, formats, units);
    setupColumnInfo<2>(columns, names, formats, units);
    setupColumnInfo<3>(columns, names, formats, units);
    setupColumnInfo<4>(columns, names, formats, units);
    setupColumnInfo<5>(columns, names, formats, units);
    setupColumnInfo<6>(columns, names, formats, units);
    setupColumnInfo<7>(columns, names, formats, units);
    setupColumnInfo<8>(columns, names, formats, units);
    setupColumnInfo<9>(columns, names, formats, units); // TODO index_sequence
    Cfitsio::CStrArray nameArray(names);
    Cfitsio::CStrArray formatArray(formats);
    Cfitsio::CStrArray unitArray(units);
    m_chrono.start();
    fits_create_tbl(
        m_fptr,
        BINARY_TBL,
        0,
        columnCount,
        nameArray.data(),
        formatArray.data(),
        unitArray.data(),
        "",
        &m_status);
    Cfitsio::CfitsioError::mayThrow(m_status);
    writeColumn<0>(columns, rowCount);
    writeColumn<1>(columns, rowCount);
    writeColumn<2>(columns, rowCount);
    writeColumn<3>(columns, rowCount);
    writeColumn<4>(columns, rowCount);
    writeColumn<5>(columns, rowCount);
    writeColumn<6>(columns, rowCount);
    writeColumn<7>(columns, rowCount);
    writeColumn<8>(columns, rowCount);
    writeColumn<9>(columns, rowCount); // TODO index_sequence
    return m_chrono.stop();
  }

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
