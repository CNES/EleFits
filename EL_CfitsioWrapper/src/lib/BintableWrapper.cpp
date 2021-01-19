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

#include <algorithm>

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Euclid {
namespace Cfitsio {
namespace Bintable {

long columnCount(fitsfile *fptr) {
  int status = 0;
  int ncols = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  mayThrowCfitsioError(status, fptr, "Cannot read the number of columns");
  return ncols;
}

long rowCount(fitsfile *fptr) {
  int status = 0;
  long nrows = 0;
  fits_get_num_rows(fptr, &nrows, &status);
  mayThrowCfitsioError(status, fptr, "Cannot read the number of rows");
  return nrows;
}

bool hasColumn(fitsfile *fptr, const std::string &name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, toCharPtr(name).get(), &index, &status);
  return (status == 0) || (status == COL_NOT_UNIQUE);
}

long columnIndex(fitsfile *fptr, const std::string &name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, toCharPtr(name).get(), &index, &status);
  mayThrowCfitsioError(status, fptr, "Cannot find index of column: " + name);
  return index;
}

namespace Internal {

template <> // TODO clean
void readColumnInfoImpl<std::string>(
    fitsfile *fptr,
    long index,
    FitsIO::VecColumn<std::string> &column,
    long rowCount) {
  column.info = readColumnInfo<std::string>(fptr, index);
  column.vector() = std::vector<std::string>(rowCount);
}

template <> // TODO clean
void readColumnChunkImpl<std::string>(
    fitsfile *fptr,
    long index,
    FitsIO::VecColumn<std::string> &column,
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
  mayThrowCfitsioError(status, fptr, "Cannot read type of column: #" + std::to_string(index - 1));
  std::vector<char *> data(rowCount);
  std::generate(data.begin(), data.end(), [&]() {
    return (char *)malloc(repeatCount);
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
  mayThrowCfitsioError(status, fptr, "Cannot read column chunk: #" + std::to_string(index - 1));
  auto columnIt = column.vector().begin() + firstRow;
  for (auto dataIt = &data[0]; dataIt != &dataIt[rowCount]; ++dataIt, ++columnIt) {
    *columnIt = std::string(*dataIt);
    free(*dataIt);
  }
}

template <>
void writeColumnChunkImpl<std::string>(
    fitsfile *fptr,
    long index,
    const FitsIO::Column<std::string> &column,
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
  mayThrowCfitsioError(
      status,
      fptr,
      "Cannot write column chunk: " + column.info.name + " (" + std::to_string(index - 1) + "); " + "rows: [" +
          std::to_string(firstRow - 1) + "-" + std::to_string(firstRow - 1 + rowCount - 1) + "]");
}

} // namespace Internal

template <>
FitsIO::VecColumn<std::string> readColumn<std::string>(fitsfile *fptr, long index) {
  long rows = rowCount(fptr);
  FitsIO::VecColumn<std::string> column(readColumnInfo<std::string>(fptr, index), rows);
  std::vector<char *> data(rows);
  std::generate(data.begin(), data.end(), [&]() {
    return (char *)malloc(column.info.repeatCount);
  });
  int status = 0;
  fits_read_col(
      fptr,
      TypeCode<std::string>::forBintable(), // datatype
      static_cast<int>(index), // colnum // column indices are int
      1, // firstrow (1-based)
      1, // firstelemn (1-based)
      column.elementCount(), // nelements
      nullptr, // nulval
      &data[0],
      nullptr, // anynul
      &status);
  mayThrowCfitsioError(status, fptr, "Cannot read string column: " + column.info.name);
  auto columnIt = column.vector().begin();
  for (auto dataIt = &data[0]; dataIt != &data[rows]; ++dataIt, ++columnIt) {
    *columnIt = std::string(*dataIt);
    free(*dataIt);
  }
  return column;
}

template <>
void writeColumn<std::string>(fitsfile *fptr, const FitsIO::Column<std::string> &column) {
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
  mayThrowCfitsioError(status, fptr, "Cannot write column: " + column.info.name);
}

} // namespace Bintable
} // namespace Cfitsio
} // namespace Euclid
