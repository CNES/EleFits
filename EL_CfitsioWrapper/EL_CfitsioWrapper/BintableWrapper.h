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

#include "EL_CfitsioWrapper/CfitsioUtils.h"
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
 * @brief Write a binary table column.
 */
template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column);

/**
 * @brief Write several bintable columns.
 */
template<typename... Ts>
void write_columns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns);

/**
 * @brief Insert a bintable column at given index.
 */
template<typename T>
void insert_column(fitsfile* fptr, std::size_t index, const FitsIO::Column<T>& column);

/**
 * @brief Insert several bintable columns at given index.
 */
template<typename... Ts>
void insert_columns(fitsfile* fptr, std::size_t index, const FitsIO::Column<Ts>&... columns);

/**
 * @brief Append a bintable column.
 */
template<typename T>
void append_column(fitsfile* fptr, const FitsIO::Column<T>& column);

/**
 * @brief Append several bintable columns.
 */
template<typename... Ts>
void append_columns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns);


///////////////
// INTERNAL //
/////////////

/// @cond INTERNAL
namespace internal {

template<typename T>
void _init_column(
    fitsfile* fptr,
    std::size_t index, std::string name,
    FitsIO::VecColumn<T>& column, std::size_t rows);

template<>
void _init_column<std::string>(
    fitsfile* fptr,
    std::size_t index, std::string name,
    FitsIO::VecColumn<std::string>& column, std::size_t rows);

template<typename T>
void _init_column(
    fitsfile* fptr,
    std::size_t index, std::string name,
    FitsIO::VecColumn<T>& column, std::size_t rows) {
  column.info.name = name;
  column.info.unit = ""; //TODO
  int typecode = 0;
  long width = 0;
  int status = 0;
  fits_get_coltype(fptr, index, &typecode, &column.info.repeat, &width, &status);
  column.vector() = std::vector<T>(rows * column.info.repeat);
}

template<int i, typename ...Ts>
struct _init_columns {
  void operator() (
      fitsfile* fptr,
      const std::vector<std::size_t>& indices, const std::vector<std::string>& names,
      std::tuple<FitsIO::VecColumn<Ts>...>& columns, std::size_t rows) {
    _init_column(fptr, indices[i], names[i], std::get<i>(columns), rows);
    _init_columns<i-1, Ts...>{}(fptr, indices, names, columns, rows);
  }
};

template<typename ...Ts>
struct _init_columns<0, Ts...> {
  void operator() (
      fitsfile* fptr,
      const std::vector<std::size_t>& indices, const std::vector<std::string>& names,
      std::tuple<FitsIO::VecColumn<Ts>...>& columns, std::size_t rows) {
    _init_column(fptr, indices[0], names[0], std::get<0>(columns), rows);
  }
};

template<typename T>
void _read_column_chunk(
    fitsfile* fptr, std::size_t index,
    FitsIO::VecColumn<T>& column,
    std::size_t first_row, std::size_t row_count);

template<>
void _read_column_chunk<std::string>(
    fitsfile* fptr, std::size_t index,
    FitsIO::VecColumn<std::string>& column,
    std::size_t first_row, std::size_t row_count);

template<typename T>
void _read_column_chunk(
    fitsfile* fptr, std::size_t index,
    FitsIO::VecColumn<T>& column,
    std::size_t first_row, std::size_t row_count) {
  int status = 0;
  auto begin = column.vector().data() + (first_row - 1) * column.info.repeat;
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

template<typename T>
void _write_column_chunk(
    fitsfile* fptr, std::size_t index,
    const FitsIO::Column<T>& column,
    std::size_t first_row, std::size_t row_count);

template<>
void _write_column_chunk<std::string>(
    fitsfile* fptr, std::size_t index,
    const FitsIO::Column<std::string>& column,
    std::size_t first_row, std::size_t row_count);

template<typename T>
void _write_column_chunk(
    fitsfile* fptr, std::size_t index,
    const FitsIO::Column<T>& column,
    std::size_t first_row, std::size_t row_count) {
  int status = 0;
  auto begin = column.data() + (first_row - 1) * column.info.repeat;
  std::size_t size = row_count * column.info.repeat;
  auto end = begin + size;
  std::vector<T> vec(begin, end);
  fits_write_col(fptr,
      TypeCode<T>::for_bintable(),
      index,
      first_row, 1, size,
      vec.data(),
      &status);
  may_throw_cfitsio_error(status, "Cannot write column chunk: "
      + column.info.name + " (" + std::to_string(index) + "); "
      + "rows: [" + std::to_string(first_row) + "-" + std::to_string(first_row + row_count - 1) + "-");
}

template<int i, typename ...Ts>
struct _write_column_chunks {
    void operator() (
        fitsfile* fptr, const std::vector<std::size_t>& indices,
        std::tuple<const FitsIO::Column<Ts>&...> columns,
        std::size_t first_row, std::size_t row_count) {
      _write_column_chunk(fptr, indices[i], std::get<i>(columns), first_row, row_count);
      _write_column_chunks<i-1, Ts...>{}(fptr, indices, columns, first_row, row_count);
    }
};

template<typename ...Ts>
struct _write_column_chunks<0, Ts...> {
    void operator() (
        fitsfile* fptr, const std::vector<std::size_t>& indices,
        std::tuple<const FitsIO::Column<Ts>&...> columns,
        std::size_t first_row, std::size_t row_count) {
      _write_column_chunk(fptr, indices[0], std::get<0>(columns), first_row, row_count);
    }
};

template<typename... Ts, std::size_t... Is>
void _write_columns(fitsfile* fptr, const std::tuple<const FitsIO::Column<Ts>&...>& columns, std14::index_sequence<Is...>) {
    write_columns<Ts...>(fptr, std::get<Is>(columns) ...);
}

}
/// @endcond


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
  FitsIO::VecColumn<T> column({ name, "", repeat }, std::vector<T>(repeat * rows)); //TODO unit
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
  internal::_init_columns<sizeof...(Ts)-1, Ts...>{}(fptr, indices, names, columns, rows);
  long chunk_rows = 0;
  fits_get_rowsize(fptr, &chunk_rows, &status);
  if (chunk_rows == 0)
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  const std::size_t urows = static_cast<std::size_t>(rows);
  for(std::size_t first=1; first<=urows; first+=chunk_rows) {
    std::size_t last = first + chunk_rows - 1;
    if (last > urows)
      chunk_rows = urows - first + 1;
    internal::_read_column_chunks<sizeof...(Ts)-1, Ts...>{}(fptr, indices, columns, first, chunk_rows);
  }
  return columns;
}

template<typename... Ts>
void write_columns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns) {
  int status = 0;
  auto table = std::forward_as_tuple(columns...);
  auto rows = std::get<0>(table).rows(); //TODO assumes all columns have same height => move to write chunks
  std::vector<std::size_t> indices { column_index(fptr, columns.info.name)... };
  long chunk_rows = 0;
  fits_get_rowsize(fptr, &chunk_rows, &status);
  if(chunk_rows == 0)
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  for(std::size_t first=1; first<=rows; first+=chunk_rows) {
    std::size_t last = first + chunk_rows - 1;
    if(last > rows)
      chunk_rows = rows - first + 1;
    internal::_write_column_chunks<sizeof...(Ts)-1, Ts...>{}(fptr, indices, table, first, chunk_rows);
  }
}

template<typename T>
void insert_column(fitsfile* fptr, std::size_t index, const FitsIO::Column<T>& column) {
  auto name = to_char_ptr(column.info.name);
  auto tform = to_char_ptr(TypeCode<T>::bintable_format(column.info.repeat));
  int status = 0;
  fits_insert_col(fptr, index, name.get(), tform.get(), &status);
  write_column(fptr, column);
}

template<typename... Ts>
void insert_columns(fitsfile* fptr, std::size_t index, const FitsIO::Column<Ts>&... columns) {
  auto names = c_str_array({ columns.info.name... });
  auto tforms = c_str_array({ TypeCode<Ts>::bintable_format(columns.info.repeat)... });
  int status = 0;
  fits_insert_cols(fptr, index, sizeof...(Ts), names.data(), tforms.data(), &status);
  write_columns(fptr, columns...);
}

template<typename T>
void append_column(fitsfile* fptr, const FitsIO::Column<T>& column) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insert_column(fptr, ncols + 1, column);
}

template<typename... Ts>
void append_columns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insert_columns(fptr, ncols + 1, columns...);
}

}
}
}

#endif
