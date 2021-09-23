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

#include "EL_CfitsioWrapper/BintableWrapper.h"

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

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
  fits_get_colnum(fptr, CASESEN, toCharPtr(name).get(), &index, &status);
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
  Cfitsio::HeaderIo::updateRecord<std::string>(fptr, { keyword, newName });
  int status = 0;
  fits_set_hdustruc(fptr, &status); // Update internal fptr state to take into account new value
  // TODO fits_set_hdustruc is DEPRECATED => ask CFitsIO support
  CfitsioError::mayThrow(status, fptr, "Cannot update name of column #" + std::to_string(index - 1));
}

long columnIndex(fitsfile* fptr, const std::string& name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, toCharPtr(name).get(), &index, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot find index of column: " + name);
  return index;
}

namespace Internal {

template <> // TODO clean
void readColumnInfoImpl<std::string>(
    fitsfile* fptr,
    long index,
    FitsIO::VecColumn<std::string>& column,
    long rowCount) {
  column.info = readColumnInfo<std::string>(fptr, index);
  column.vector() = std::vector<std::string>(rowCount);
}

template <> // TODO clean
void readColumnChunkImpl<std::string>(
    fitsfile* fptr,
    long index,
    FitsIO::VecColumn<std::string>& column,
    long firstRow,
    long rowCount) {
  int status = 0;
  long repeatCount = 0;
  fits_get_coltype(
      fptr,
      static_cast<int>(index), // column indices are int
      nullptr,
      &repeatCount,
      nullptr,
      &status); // TODO wrap?
  CfitsioError::mayThrow(status, fptr, "Cannot read type of column: #" + std::to_string(index - 1));
  std::vector<char*> data(rowCount);
  std::generate(data.begin(), data.end(), [&]() {
    return (char*)malloc(repeatCount);
  });
  fits_read_col(
      fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      firstRow,
      1,
      rowCount,
      nullptr,
      data.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read column chunk: #" + std::to_string(index - 1));
  auto columnIt = column.vector().begin() + firstRow - 1;
  for (auto dataIt = data.begin(); dataIt != data.end(); ++dataIt, ++columnIt) {
    *columnIt = std::string(*dataIt);
    free(*dataIt);
  }
}

template <>
void writeColumnChunkImpl<std::string>(
    fitsfile* fptr,
    long index,
    const FitsIO::Column<std::string>& column,
    long firstRow,
    long rowCount) {
  int status = 0;
  auto begin = column.data() + (firstRow - 1);
  long size = rowCount;
  auto end = begin + size;
  CStrArray array(begin, end);
  fits_write_col(
      fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      firstRow,
      1,
      size,
      array.data(),
      &status);
  CfitsioError::mayThrow(
      status,
      fptr,
      "Cannot write column chunk: " + column.info.name + " (" + std::to_string(index - 1) + "); " + "rows: [" +
          std::to_string(firstRow - 1) + "-" + std::to_string(firstRow - 1 + rowCount - 1) + "]");
}

} // namespace Internal

template <>
void readColumnSegment<std::string>(
    fitsfile* fptr,
    const FitsIO::Segment& rows,
    long index,
    FitsIO::Column<std::string>& column) {
  std::vector<char*> data(rows.size());
  std::generate(data.begin(), data.end(), [&]() {
    return (char*)malloc(column.info.repeatCount);
  });
  int status = 0;
  fits_read_col(
      fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index),
      rows.lower,
      1, // firstelemn (1-based)
      data.size(), // nelements = number of rows for strings
      nullptr, // nulval
      &data[0],
      nullptr, // anynul
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read string column #" + std::to_string(index));
  auto columnIt = column.data();
  for (auto dataIt = data.begin(); dataIt != data.end(); ++dataIt, ++columnIt) {
    *columnIt = std::string(*dataIt);
    free(*dataIt);
  }
}

template <>
void writeColumn<std::string>(fitsfile* fptr, const FitsIO::Column<std::string>& column) {
  const auto begin = column.data();
  const auto end = begin + column.elementCount();
  CStrArray array(begin, end);
  long index = columnIndex(fptr, column.info.name);
  int status = 0;
  fits_write_col(
      fptr,
      TypeCode<std::string>::forBintable(), // datatype
      static_cast<int>(index), // colnum // column indices are int
      1, // firstrow (1-based)
      1, // firstelem (1-based)
      column.elementCount(), // nelements
      array.data(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write column: " + column.info.name);
}

template <>
void writeColumnSegment(fitsfile* fptr, long firstRow, const FitsIO::Column<std::string>& column) {
  const auto begin = column.data();
  const auto end = begin + column.elementCount();
  CStrArray array(begin, end);
  long index = columnIndex(fptr, column.info.name);
  int status = 0;
  fits_write_col(
      fptr,
      TypeCode<std::string>::forBintable(), // datatype
      static_cast<int>(index), // colnum // column indices are int
      firstRow + 1, // firstrow (1-based)
      1, // firstelem (1-based)
      column.elementCount(), // nelements
      array.data(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write column: " + column.info.name);
}

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid
