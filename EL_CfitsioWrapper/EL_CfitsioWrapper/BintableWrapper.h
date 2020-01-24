/**
 * @file EL_CfitsioWrapper/BintableWrapper.h
 * @date 07/27/19
 * @author user
 *
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

#ifndef _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H
#define _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H

#include <tuple>
#include <vector>

#include "EL_FitsData/Column.h"

#include "EL_CfitsioWrapper/TypeWrapper.h"


namespace Euclid {
namespace Cfitsio {

/**
 * @brief Bintable-related functions.
 */
namespace Bintable {

/**
 * @brief Get the index of a Bintable column.
 */
std::size_t column_index(fitsfile* fptr, std::string name);

/**
 * @brief Read a Bintable column with given name.
 */
template<typename T>
FitsIO::VecColumn<T> read_column(fitsfile* fptr, std::string name);

/**
 * @brief Read several Bintable columns with given names.
 */
template<typename ...Ts>
std::tuple<FitsIO::VecColumn<Ts>...> read_columns(fitsfile* fptr, std::vector<std::string> names);

/**
 * @brief Write a binary table column with given name.
 */
template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column);


///////////////
// INTERNAL //
/////////////


namespace internal {

template<typename T>
void _init_column(fitsfile* fptr, std::string name, FitsIO::VecColumn<T>& column, std::size_t rows) {
  column.info.name = name;
  column.info.unit = ""; //TODO
  column.info.repeat = 1; //TODO
  column.vector() = std::vector<T>(rows * column.info.repeat);
}

template<int i, typename ...Ts>
struct _init_columns {
  void operator() (fitsfile* fptr, std::vector<std::string> names, std::tuple<FitsIO::VecColumn<Ts>...>& columns, std::size_t rows) {
    _init_column(fptr, names[i], std::get<i>(columns), rows);
    _init_columns<i-1, Ts...>{}(fptr, names, columns, rows);
  }
};

template<typename ...Ts>
struct _init_columns<0, Ts...> {
  void operator() (fitsfile* fptr, std::vector<std::string> names, std::tuple<FitsIO::VecColumn<Ts>...>& columns, std::size_t rows) {
    _init_column(fptr, names[0], std::get<0>(columns), rows);
  }
};

template<typename T>
void _read_column_chunk(
    fitsfile* fptr, std::size_t index,
    FitsIO::VecColumn<T>& column,
    std::size_t first_row, std::size_t row_count) {
  int status = 0;
  auto begin = column.vector().data() + first_row - 1;
  fits_read_col(fptr,
      TypeCode<T>::for_bintable(),
      index,
      first_row, 1, row_count * column.info.repeat,
      nullptr,
      begin,
      nullptr,
      &status);
}

template<int i, typename ...Ts>
struct _read_column_chunks {
    void operator() (
        fitsfile* fptr, const std::vector<std::size_t>& indices,
        std::tuple<FitsIO::VecColumn<Ts>...>& columns,
        std::size_t first_row, std::size_t row_count) {
      _read_column_chunk(fptr, indices[i], std::get<i>(columns), first_row, row_count);
      _read_column_chunks<i-1, Ts...>{}(fptr, indices, columns, first_row, row_count);
    }
};

template<typename ...Ts>
struct _read_column_chunks<0, Ts...> {
    void operator() (
        fitsfile* fptr, const std::vector<std::size_t>& indices,
        std::tuple<FitsIO::VecColumn<Ts>...>& columns,
        std::size_t first_row, std::size_t row_count) {
      _read_column_chunk(fptr, indices[0], std::get<0>(columns), first_row, row_count);
    }
};

}


/////////////////////
// IMPLEMENTATION //
///////////////////


template<>
FitsIO::VecColumn<std::string> read_column<std::string>(fitsfile* fptr, std::string name);

template<>
void write_column<std::string>(fitsfile* fptr, const FitsIO::Column<std::string>& column);

template<typename T>
FitsIO::VecColumn<T> read_column(fitsfile* fptr, std::string name) {
  std::size_t index = column_index(fptr, name);
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  long rows = 0;
  int status = 0;
  fits_get_coltype(fptr, index, &typecode, &repeat, &width, &status);
  fits_get_num_rows(fptr, &rows, &status);
  may_throw_cfitsio_error(status, "Cannot read column dimensions");
  FitsIO::VecColumn<T> column({ name, "", repeat }, std::vector<T>(repeat * rows));
  fits_read_col(
    fptr,
    TypeCode<T>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelemn (1-based)
    column.nelements(), // nelements
    nullptr, // nulval
    column.data(),
    nullptr, // anynul
    &status
  );
  may_throw_cfitsio_error(status, "Cannot read column data");
  return column;
}


template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column) {
  size_t index = column_index(fptr, column.info.name);
  const auto begin = column.data();
  const auto end = begin + column.nelements();
  std::vector<T> nonconst_data(begin, end); // We need a non-const data for CFitsIO
  //TODO avoid copy
  int status = 0;
  fits_write_col(
    fptr,
    TypeCode<T>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelem (1-based)
    column.nelements(), // nelements
    nonconst_data.data(),
    &status
    );
  may_throw_cfitsio_error(status, "Cannot write column data");
}

template<typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> read_columns(fitsfile* fptr, std::vector<std::string> names) {
  std::vector<std::size_t> indices(names.size());
  for(std::size_t c=0; c<names.size(); ++c)
    indices[c] = column_index(fptr, names[c]);
  std::tuple<FitsIO::VecColumn<Ts>...> columns;
  int status = 0;
  long rows = 0;
  fits_get_num_rows(fptr, &rows, &status);
  internal::_init_columns<sizeof...(Ts)-1, Ts...>{}(fptr, names, columns, rows);
  long chunk_rows = 0;
  fits_get_rowsize(fptr, &chunk_rows, &status);
  if(chunk_rows == 0)
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  for(std::size_t first=1; first<=rows; first+=chunk_rows) {
    std::size_t last = first + chunk_rows - 1;
    if(last > rows)
      chunk_rows = rows - first + 1;
    internal::_read_column_chunks<sizeof...(Ts)-1, Ts...>{}(fptr, indices, columns, first, chunk_rows);
  }
  return columns;
}

}
}
}

#endif
