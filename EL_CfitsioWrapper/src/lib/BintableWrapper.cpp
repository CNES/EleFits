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

long columnIndex(fitsfile* fptr, const std::string& name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, toCharPtr(name).get(), &index, &status);
  mayThrowCfitsioError(status);
  return index;
}

/// @cond internal
namespace internal {

template<> //TODO clean
void initColumnImpl<std::string>(
    fitsfile* fptr,
    long index, const std::string& name,
    FitsIO::VecColumn<std::string>& column, long rows) {
  column.info.name = name;
  column.info.unit = ""; //TODO
  int typecode = 0;
  long width = 0;
  int status = 0;
  fits_get_coltype(
      fptr, static_cast<int>(index), // column indices are int
      &typecode, &column.info.repeat, &width, &status);
  column.vector() = std::vector<std::string>(rows);
}

template<> //TODO clean
void readColumnChunkImpl<std::string>(
    fitsfile* fptr, long index,
    FitsIO::VecColumn<std::string>& column,
    long firstRow, long rowCount) {
  int status = 0;
  long repeat = 0;
  fits_get_coltype(
      fptr, static_cast<int>(index), // column indices are int
      nullptr, &repeat, nullptr, &status); //TODO wrap?
  mayThrowCfitsioError(status);
  std::vector<char*> data(rowCount);
  for(long i = 0; i < rowCount; ++i) //TODO iterator
    data[i] = (char*) malloc(repeat);
  fits_read_col(fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      firstRow, 1, rowCount,
      nullptr,
      data.data(),
      nullptr,
      &status);
  for(long i = 0; i < rowCount; ++i) {
    column.vector()[i + firstRow] = std::string(data[i]);
    free(data[i]);
  }
}

template<>
void writeColumnChunkImpl<std::string>(
    fitsfile* fptr, long index,
    const FitsIO::Column<std::string>& column,
    long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.data() + (firstRow - 1);
  long size = rowCount;
  auto end = begin + size;
  CStrArray array(begin, end);
  fits_write_col(fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      firstRow, 1, size,
      array.data(),
      &status);
  mayThrowCfitsioError(status, "Cannot write column chunk: "
      + column.info.name + " (" + std::to_string(index) + "); "
      + "rows: [" + std::to_string(firstRow) + "-" + std::to_string(firstRow + rowCount - 1) + "-");
}

}
/// @endcond

template<>
FitsIO::VecColumn<std::string> readColumn<std::string>(fitsfile* fptr, const std::string& name) {
  long index = columnIndex(fptr, name);
  long rows = 0;
  int status = 0;
  fits_get_num_rows(fptr, &rows, &status);
  mayThrowCfitsioError(status);
  long repeat = 0;
  fits_get_coltype(
    fptr, static_cast<int>(index), // column indices are int
    nullptr, &repeat, nullptr, &status); //TODO wrap?
  mayThrowCfitsioError(status);
  std::vector<char*> data(rows);
  for(long i = 0; i < rows; ++i) //TODO iterator
    data[i] = (char*) malloc(repeat);
  FitsIO::VecColumn<std::string> column({name, "", repeat}, std::vector<std::string>(rows)); //TODO unit
  fits_read_col(
    fptr,
    TypeCode<std::string>::forBintable(), // datatype
    static_cast<int>(index), // colnum // column indices are int
    1, // firstrow (1-based)
    1, // firstelemn (1-based)
    column.nelements(), // nelements
    nullptr, // nulval
    &data[0],
    nullptr, // anynul
    &status
  );
  mayThrowCfitsioError(status);
  for(long i = 0; i < rows; ++i) {
    column.vector()[i] = std::string(data[i]);
    free(data[i]);
  }
  return column;
}

template<>
void writeColumn<std::string>(fitsfile* fptr, const FitsIO::Column<std::string>& column) {
  const auto begin = column.data();
  const auto end = begin + column.nelements();
  CStrArray array(begin, end);
  long index = columnIndex(fptr, column.info.name);
  int status = 0;
  fits_write_col(
    fptr,
    TypeCode<std::string>::forBintable(), // datatype
    static_cast<int>(index), // colnum // column indices are int
    1, // firstrow (1-based)
    1, // firstelem (1-based)
    column.nelements(), // nelements
    array.data(),
    &status
    );
  mayThrowCfitsioError(status);
}

}
}
}
