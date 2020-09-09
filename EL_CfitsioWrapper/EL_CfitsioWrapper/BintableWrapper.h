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
long columnIndex(fitsfile *fptr, const std::string &name);

/**
 * @brief Read a Bintable column with given name.
 */
template <typename T>
FitsIO::VecColumn<T> readColumn(fitsfile *fptr, const std::string &name);

/**
 * @brief Read several Bintable columns with given names.
 */
template <typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> readColumns(fitsfile *fptr, const std::vector<std::string> &names);

/**
 * @brief Write a binary table column.
 */
template <typename T>
void writeColumn(fitsfile *fptr, const FitsIO::Column<T> &column);

/**
 * @brief Write several bintable columns.
 */
template <typename... Ts>
void writeColumns(fitsfile *fptr, const FitsIO::Column<Ts> &... columns);

/**
 * @brief Insert a bintable column at given index.
 */
template <typename T>
void insertColumn(fitsfile *fptr, long index, const FitsIO::Column<T> &column);

/**
 * @brief Insert several bintable columns at given index.
 */
template <typename... Ts>
void insertColumns(fitsfile *fptr, long index, const FitsIO::Column<Ts> &... columns);

/**
 * @brief Append a bintable column.
 */
template <typename T>
void appendColumn(fitsfile *fptr, const FitsIO::Column<T> &column);

/**
 * @brief Append several bintable columns.
 */
template <typename... Ts>
void appendColumns(fitsfile *fptr, const FitsIO::Column<Ts> &... columns);

///////////////
// INTERNAL //
/////////////

/// @cond INTERNAL
namespace Internal {

template <typename T>
void initColumnImpl(fitsfile *fptr, long index, const std::string &name, FitsIO::VecColumn<T> &column, long rows);

template <>
void initColumnImpl<std::string>(
    fitsfile *fptr,
    long index,
    const std::string &name,
    FitsIO::VecColumn<std::string> &column,
    long rows);

template <typename T>
void initColumnImpl(fitsfile *fptr, long index, const std::string &name, FitsIO::VecColumn<T> &column, long rows) {
  column.info.name = name;
  column.info.unit = ""; // TODO
  int typecode = 0;
  long width = 0;
  int status = 0;
  fits_get_coltype(fptr, static_cast<int>(index), &typecode, &column.info.repeat, &width, &status);
  column.vector() = std::vector<T>(rows * column.info.repeat);
}

template <std::size_t i, typename... Ts>
struct InitColumnsImpl {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      const std::vector<std::string> &names,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long rows) {
    initColumnImpl(fptr, indices[i], names[i], std::get<i>(columns), rows);
    InitColumnsImpl<i - 1, Ts...> {}(fptr, indices, names, columns, rows);
  }
};

template <typename... Ts>
struct InitColumnsImpl<0, Ts...> {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      const std::vector<std::string> &names,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long rows) {
    initColumnImpl(fptr, indices[0], names[0], std::get<0>(columns), rows);
  }
};

template <typename T>
void readColumnChunkImpl(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long firstRow, long rowCount);

template <>
void readColumnChunkImpl<std::string>(
    fitsfile *fptr,
    long index,
    FitsIO::VecColumn<std::string> &column,
    long firstRow,
    long rowCount);

template <typename T>
void readColumnChunkImpl(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.vector().data() + (firstRow - 1) * column.info.repeat;
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(),
      static_cast<int>(index),
      firstRow,
      1,
      rowCount * column.info.repeat,
      nullptr,
      begin,
      nullptr,
      &status);
}

template <std::size_t i, typename... Ts>
struct ReadColumnChunksImpl {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long firstRow,
      long rowCount) {
    readColumnChunkImpl(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    ReadColumnChunksImpl<i - 1, Ts...> {}(fptr, indices, columns, firstRow, rowCount);
  }
};

template <typename... Ts>
struct ReadColumnChunksImpl<0, Ts...> {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long firstRow,
      long rowCount) {
    readColumnChunkImpl(fptr, indices[0], std::get<0>(columns), firstRow, rowCount);
  }
};

template <typename T>
void writeColumnChunkImpl(fitsfile *fptr, long index, const FitsIO::Column<T> &column, long firstRow, long rowCount);

template <>
void writeColumnChunkImpl<std::string>(
    fitsfile *fptr,
    long index,
    const FitsIO::Column<std::string> &column,
    long firstRow,
    long rowCount);

template <typename T>
void writeColumnChunkImpl(fitsfile *fptr, long index, const FitsIO::Column<T> &column, long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.data() + (firstRow - 1) * column.info.repeat;
  long size = rowCount * column.info.repeat;
  auto end = begin + size;
  std::vector<T> vec(begin, end);
  fits_write_col(fptr, TypeCode<T>::forBintable(), static_cast<int>(index), firstRow, 1, size, vec.data(), &status);
  mayThrowCfitsioError(
      status,
      "Cannot write column chunk: " + column.info.name + " (" + std::to_string(index) + "); " + "rows: [" +
          std::to_string(firstRow) + "-" + std::to_string(firstRow + rowCount - 1) + "-");
}

template <std::size_t i, typename... Ts>
struct WriteColumnChunksImpl {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<const FitsIO::Column<Ts> &...> columns,
      long firstRow,
      long rowCount) {
    writeColumnChunkImpl(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    WriteColumnChunksImpl<i - 1, Ts...> {}(fptr, indices, columns, firstRow, rowCount);
  }
};

template <typename... Ts>
struct WriteColumnChunksImpl<0, Ts...> {
  void operator()(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<const FitsIO::Column<Ts> &...> columns,
      long firstRow,
      long rowCount) {
    writeColumnChunkImpl(fptr, indices[0], std::get<0>(columns), firstRow, rowCount);
  }
};

template <typename... Ts, std::size_t... Is>
void writeColumnsImpl(
    fitsfile *fptr,
    const std::tuple<const FitsIO::Column<Ts> &...> &columns,
    std14::index_sequence<Is...>) {
  writeColumns<Ts...>(fptr, std::get<Is>(columns)...);
} // TODO used?

} // namespace Internal
/// @endcond

/////////////////////
// IMPLEMENTATION //
///////////////////

template <>
FitsIO::VecColumn<std::string> readColumn<std::string>(fitsfile *fptr, const std::string &name);

template <>
void writeColumn<std::string>(fitsfile *fptr, const FitsIO::Column<std::string> &column);

template <typename T>
FitsIO::VecColumn<T> readColumn(fitsfile *fptr, const std::string &name) {
  /* Read metadata */
  long index = columnIndex(fptr, name);
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  long rows = 0;
  int status = 0;
  fits_get_coltype(fptr, static_cast<int>(index), &typecode, &repeat, &width, &status);
  fits_get_num_rows(fptr, &rows, &status);
  mayThrowCfitsioError(status, "Cannot read column dimensions");
  FitsIO::VecColumn<T> column({ name, "", repeat },
                              std::vector<T>(repeat * rows)); // TODO unit
  /* Read data */
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      1, // firstrow (1-based)
      1, // firstelemn (1-based)
      column.nelements(), // nelements
      nullptr, // nulval
      column.data(),
      nullptr, // anynul
      &status);
  mayThrowCfitsioError(status, "Cannot read column data");
  return column;
}

template <typename T>
void writeColumn(fitsfile *fptr, const FitsIO::Column<T> &column) {
  long index = columnIndex(fptr, column.info.name);
  const auto begin = column.data();
  const auto end = begin + column.nelements();
  std::vector<T> nonconstData(begin, end); // We need a non-const data for CFitsIO
  int status = 0;
  fits_write_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      1, // firstrow (1-based)
      1, // firstelem (1-based)
      column.nelements(), // nelements
      nonconstData.data(),
      &status);
  mayThrowCfitsioError(status, "Cannot write column data");
}

template <typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> readColumns(fitsfile *fptr, const std::vector<std::string> &names) {
  /* List column indices */
  std::vector<long> indices(names.size());
  for (std::size_t c = 0; c < names.size(); ++c) { // TODO iterator
    indices[c] = columnIndex(fptr, names[c]);
  }
  /* Read row count */
  int status = 0;
  long rows = 0;
  fits_get_num_rows(fptr, &rows, &status);
  /* Read column metadata */
  std::tuple<FitsIO::VecColumn<Ts>...> columns;
  Internal::InitColumnsImpl<sizeof...(Ts) - 1, Ts...> {}(fptr, indices, names, columns, rows);
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  if (chunkRows == 0) {
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  }
  /* Read column data */
  for (long first = 1; first <= rows; first += chunkRows) {
    long last = first + chunkRows - 1;
    if (last > rows) {
      chunkRows = rows - first + 1;
    }
    Internal::ReadColumnChunksImpl<sizeof...(Ts) - 1, Ts...> {}(fptr, indices, columns, first, chunkRows);
  }
  return columns;
}

template <typename... Ts>
void writeColumns(fitsfile *fptr, const FitsIO::Column<Ts> &... columns) {
  int status = 0;
  /* Get chunk size */
  auto table = std::forward_as_tuple(columns...);
  auto rows = std::get<0>(table).rows(); // TODO assumes all columns have
                                         // same height => move to write
                                         // chunks
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  if (chunkRows == 0) {
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  }
  /* Write column data */
  std::vector<long> indices { columnIndex(fptr, columns.info.name)... };
  for (long first = 1; first <= rows; first += chunkRows) {
    long last = first + chunkRows - 1;
    if (last > rows) {
      chunkRows = rows - first + 1;
    }
    Internal::WriteColumnChunksImpl<sizeof...(Ts) - 1, Ts...> {}(fptr, indices, table, first, chunkRows);
  }
}

template <typename T>
void insertColumn(fitsfile *fptr, long index, const FitsIO::Column<T> &column) {
  auto name = toCharPtr(column.info.name);
  auto tform = toCharPtr(TypeCode<T>::tform(column.info.repeat));
  int status = 0;
  fits_insert_col(fptr, static_cast<int>(index), name.get(), tform.get(), &status);
  writeColumn(fptr, column);
}

template <typename... Ts>
void insertColumns(fitsfile *fptr, long index, const FitsIO::Column<Ts> &... columns) {
  auto names = CStrArray({ columns.info.name... });
  auto tforms = CStrArray({ TypeCode<Ts>::tform(columns.info.repeat)... });
  int status = 0;
  fits_insert_cols(fptr, static_cast<int>(index), sizeof...(Ts), names.data(), tforms.data(), &status);
  writeColumns(fptr, columns...);
}

template <typename T>
void appendColumn(fitsfile *fptr, const FitsIO::Column<T> &column) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insertColumn(fptr, ncols + 1, column);
}

template <typename... Ts>
void appendColumns(fitsfile *fptr, const FitsIO::Column<Ts> &... columns) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insertColumns(fptr, ncols + 1, columns...);
}

} // namespace Bintable
} // namespace Cfitsio
} // namespace Euclid

#endif
