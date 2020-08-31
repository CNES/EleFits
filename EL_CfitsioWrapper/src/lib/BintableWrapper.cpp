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

long column_index(fitsfile* fptr, const std::string& name) {
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, to_char_ptr(name).get(), &index, &status);
  may_throw_cfitsio_error(status);
  return index;
}

/// @cond internal
namespace internal {

template<> //TODO clean
void _init_column<std::string>(
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
void _read_column_chunk<std::string>(
    fitsfile* fptr, long index,
    FitsIO::VecColumn<std::string>& column,
    long first_row, long row_count) {
  int status = 0;
  long repeat = 0;
  fits_get_coltype(
      fptr, static_cast<int>(index), // column indices are int
      nullptr, &repeat, nullptr, &status); //TODO wrap?
  may_throw_cfitsio_error(status);
  std::vector<char*> data(row_count);
  for(long i = 0; i < row_count; ++i) //TODO iterator
    data[i] = (char*) malloc(repeat);
  fits_read_col(fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      first_row, 1, row_count,
      nullptr,
      data.data(),
      nullptr,
      &status);
  for(long i = 0; i < row_count; ++i) {
    column.vector()[i + first_row] = std::string(data[i]);
    free(data[i]);
  }
}

template<>
void _write_column_chunk<std::string>(
    fitsfile* fptr, long index,
    const FitsIO::Column<std::string>& column,
    long first_row, long row_count) {
  int status = 0;
  auto begin = column.data() + (first_row - 1);
  long size = row_count;
  auto end = begin + size;
  c_str_array array(begin, end);
  fits_write_col(fptr,
      TypeCode<std::string>::forBintable(),
      static_cast<int>(index), // column indices are int
      first_row, 1, size,
      array.data(),
      &status);
  may_throw_cfitsio_error(status, "Cannot write column chunk: "
      + column.info.name + " (" + std::to_string(index) + "); "
      + "rows: [" + std::to_string(first_row) + "-" + std::to_string(first_row + row_count - 1) + "-");
}

}
/// @endcond

template<>
FitsIO::VecColumn<std::string> read_column<std::string>(fitsfile* fptr, const std::string& name) {
  long index = column_index(fptr, name);
  long rows = 0;
  int status = 0;
  fits_get_num_rows(fptr, &rows, &status);
  may_throw_cfitsio_error(status);
  long repeat = 0;
  fits_get_coltype(
    fptr, static_cast<int>(index), // column indices are int
    nullptr, &repeat, nullptr, &status); //TODO wrap?
  may_throw_cfitsio_error(status);
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
  may_throw_cfitsio_error(status);
  for(long i = 0; i < rows; ++i) {
    column.vector()[i] = std::string(data[i]);
    free(data[i]);
  }
  return column;
}

template<>
void write_column<std::string>(fitsfile* fptr, const FitsIO::Column<std::string>& column) {
  const auto begin = column.data();
  const auto end = begin + column.nelements();
  c_str_array array(begin, end);
  long index = column_index(fptr, column.info.name);
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
  may_throw_cfitsio_error(status);
}

}
}
}
