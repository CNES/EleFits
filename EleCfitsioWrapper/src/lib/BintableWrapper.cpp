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

#include "EleCfitsioWrapper/BintableWrapper.h"

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsUtils/StringUtils.h"

#include <algorithm>

namespace Euclid {
namespace Cfitsio {
namespace BintableIo {

long columnCount(fitsfile* fptr) {
  int status = 0;
  int ncols = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read the number of columns");
  return ncols;
}

long rowCount(fitsfile* fptr) {
  int status = 0;
  long nrows = 0;
  fits_get_num_rows(fptr, &nrows, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read the number of rows");
  return nrows;
}

bool hasColumn(fitsfile* fptr, const std::string& name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, Fits::String::toCharPtr(name).get(), &index, &status);
  return (status == 0) || (status == COL_NOT_UNIQUE);
}

std::string columnName(fitsfile* fptr, long index) {
  int status = 0;
  char ttype[FLEN_VALUE];
  fits_get_bcolparms(
      fptr,
      index, // 1-based here
      ttype,
      nullptr, // tunit
      nullptr, // dtype
      nullptr, // repeat
      nullptr, // tscal
      nullptr, // tzero
      nullptr, // tnull
      nullptr, // tdisp
      &status);
  // TODO Should we just read TTYPEn instead ?
  CfitsioError::mayThrow(status, fptr, "Cannot find name of column: " + std::to_string(index - 1));
  return ttype;
}

void updateColumnName(fitsfile* fptr, long index, const std::string& newName) {
  const std::string keyword = "TTYPE" + std::to_string(index);
  Cfitsio::HeaderIo::updateRecord<std::string>(fptr, {keyword, newName});
  int status = 0;
  fits_set_hdustruc(fptr, &status); // Update internal fptr state to take into account new value
  // TODO fits_set_hdustruc is DEPRECATED => ask CFitsIO support
  CfitsioError::mayThrow(status, fptr, "Cannot update name of column #" + std::to_string(index - 1));
}

long columnIndex(fitsfile* fptr, const std::string& name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, Fits::String::toCharPtr(name).get(), &index, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot find index of column: " + name);
  return index;
}

namespace Internal {

template <> // TODO clean
void readColumnInfoImpl(fitsfile* fptr, long index, Fits::VecColumn<std::string>& column, long rowCount) {
  column = Fits::VecColumn<std::string>(readColumnInfo<std::string>(fptr, index), std::vector<std::string>(rowCount));
}

} // namespace Internal

template <>
void readColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, std::string* data) {
  int status = 0;
  std::vector<char*> vec(rows.size());
  std::generate(vec.begin(), vec.end(), [&]() {
    return (char*)malloc(repeatCount);
  });
  fits_read_col(
      fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      rows.front,
      1,
      rows.size(),
      nullptr,
      vec.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read column data: #" + std::to_string(index - 1));
  auto columnIt = data;
  for (auto vecIt = vec.begin(); vecIt != vec.end(); ++vecIt, ++columnIt) {
    *columnIt = std::string(*vecIt);
    free(*vecIt);
  }
}

template <>
void writeColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, const std::string* data) {
  int status = 0;
  Fits::String::CStrArray array(data, data + rows.size());
  fits_write_col(
      fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      rows.front,
      1,
      rows.size(),
      array.data(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write column data: #" + std::to_string(index - 1));
}

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid
