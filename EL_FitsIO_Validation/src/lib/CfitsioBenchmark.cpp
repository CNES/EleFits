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

#include "EL_FitsIO_Validation/CfitsioBenchmark.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

CfitsioBenchmark::CfitsioBenchmark(const std::string& filename) : Benchmark(), m_fptr(nullptr), m_status(0) {
  fits_create_file(&m_fptr, (std::string("!") + filename).c_str(), &m_status);
}

BChronometer::Unit CfitsioBenchmark::writeImage(const BRaster& raster) {
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

BChronometer::Unit CfitsioBenchmark::writeBintable(const BColumns& columns) {
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

} // namespace Test
} // namespace FitsIO
} // namespace Euclid
