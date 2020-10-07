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

#ifdef _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_IMPL

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Bintable {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Read metadata and allocate data.
 */
template <typename T>
void readInfo(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void readInfo<std::string>(fitsfile *fptr, long index, FitsIO::VecColumn<std::string> &column, long rowCount);

template <typename T>
void readInfo(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long rowCount) {
  column.info = readColumnInfo<T>(fptr, index);
  column.vector() = std::vector<T>(column.info.repeatCount * rowCount);
}

/**
 * @brief Read a column chunk.
 */
template <typename T>
void readChunk(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long firstRow, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void readChunk<std::string>(
    fitsfile *fptr,
    long index,
    FitsIO::VecColumn<std::string> &column,
    long firstRow,
    long rowCount);

template <typename T>
void readChunk(fitsfile *fptr, long index, FitsIO::VecColumn<T> &column, long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.vector().data() + (firstRow - 1) * column.info.repeatCount;
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(),
      static_cast<int>(index),
      firstRow,
      1,
      rowCount * column.info.repeatCount,
      nullptr,
      begin,
      nullptr,
      &status);
  mayThrowCfitsioError(
      status,
      "Cannot read column chunk: " + column.info.name + " (" + std::to_string(index) + "); " + "rows: [" +
          std::to_string(firstRow) + "-" + std::to_string(firstRow + rowCount - 1) + "-");
}

/**
 * @brief Write a column chunk.
 */
template <typename T>
void writeChunk(fitsfile *fptr, long index, const FitsIO::Column<T> &column, long firstRow, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void writeChunk<std::string>(
    fitsfile *fptr,
    long index,
    const FitsIO::Column<std::string> &column,
    long firstRow,
    long rowCount);

template <typename T>
void writeChunk(fitsfile *fptr, long index, const FitsIO::Column<T> &column, long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.data() + (firstRow - 1) * column.info.repeatCount;
  long size = rowCount * column.info.repeatCount;
  auto end = begin + size;
  std::vector<T> vec(begin, end);
  fits_write_col(fptr, TypeCode<T>::forBintable(), static_cast<int>(index), firstRow, 1, size, vec.data(), &status);
  mayThrowCfitsioError(
      status,
      "Cannot write column chunk: " + column.info.name + " (" + std::to_string(index) + "); " + "rows: [" +
          std::to_string(firstRow) + "-" + std::to_string(firstRow + rowCount - 1) + "-");
}

/**
 * @brief Helper class to loop on a collection of columns.
 */
template <std::size_t i, typename... Ts>
struct ColumnLooper {

  /** @brief Read metadata and allocate data for each column */
  static void readInfos(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long rowCount) {
    readInfo(fptr, indices[i], std::get<i>(columns), rowCount);
    ColumnLooper<i - 1, Ts...>::readInfos(fptr, indices, columns, rowCount);
  }

  /** @brief Read a chunk of each column */
  static void readChunks(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long firstRow,
      long rowCount) {
    readChunk(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    ColumnLooper<i - 1, Ts...>::readChunks(fptr, indices, columns, firstRow, rowCount);
  }

  /** @brief Write a chunk of each colum */
  static void writeChunks(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<const FitsIO::Column<Ts> &...> columns,
      long firstRow,
      long rowCount) {
    writeChunk(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    ColumnLooper<i - 1, Ts...>::writeChunks(fptr, indices, columns, firstRow, rowCount);
  }
};

/**
 * @brief Terminal case.
 */
template <typename... Ts>
struct ColumnLooper<0, Ts...> {

  /** @brief Read metadata and allocate data for the first column */
  static void readInfos(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long rowCount) {
    readInfo(fptr, indices[0], std::get<0>(columns), rowCount);
  }

  /** @brief Read a chunk of the first column */
  static void readChunks(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<FitsIO::VecColumn<Ts>...> &columns,
      long firstRow,
      long rowCount) {
    readChunk(fptr, indices[0], std::get<0>(columns), firstRow, rowCount);
  }

  /** @brief Write a chunk of the first colum */
  static void writeChunks(
      fitsfile *fptr,
      const std::vector<long> &indices,
      std::tuple<const FitsIO::Column<Ts> &...> columns,
      long firstRow,
      long rowCount) {
    writeChunk(fptr, indices[0], std::get<0>(columns), firstRow, rowCount);
  }
};

} // namespace Internal
/// @endcond

template <typename T>
FitsIO::ColumnInfo<T> readColumnInfo(fitsfile *fptr, long index) {
  FitsIO::ColumnInfo<T> info;
  int status = 0;
  char name[FLEN_VALUE];
  char unit[FLEN_VALUE];
  fits_get_bcolparms(
      fptr,
      index,
      name,
      unit,
      nullptr, // typechar
      &info.repeatCount,
      nullptr, // scale
      nullptr, // zero
      nullptr, // nulval
      nullptr, // tdisp
      &status);
  mayThrowCfitsioError(status, "Cannot read column metadata: #" + std::to_string(index));
  info.name.assign(name);
  info.unit.assign(unit);
  return info;
}

/**
 * @brief String specialization.
 */
template <>
FitsIO::VecColumn<std::string> readColumn<std::string>(fitsfile *fptr, const std::string &name);

/**
 * @brief String specialization.
 */
template <>
void writeColumn<std::string>(fitsfile *fptr, const FitsIO::Column<std::string> &column);

template <typename T>
FitsIO::VecColumn<T> readColumn(fitsfile *fptr, const std::string &name) {
  /* Read metadata */
  long index = columnIndex(fptr, name);
  long rows = rowCount(fptr);
  FitsIO::VecColumn<T> column(readColumnInfo<T>(fptr, index), rows);
  int status = 0;
  /* Read data */
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      1, // firstrow (1-based)
      1, // firstelemn (1-based)
      column.elementCount(), // nelements
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
  const auto end = begin + column.elementCount();
  std::vector<T> nonconstData(begin, end); // We need a non-const data for CFitsIO
  int status = 0;
  fits_write_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      1, // firstrow (1-based)
      1, // firstelem (1-based)
      column.elementCount(), // nelements
      nonconstData.data(),
      &status);
  mayThrowCfitsioError(status, "Cannot write column data");
}

template <typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> readColumns(fitsfile *fptr, const std::vector<std::string> &names) {
  /* List column indices */
  std::vector<long> indices(names.size());
  for (std::size_t c = 0; c < names.size(); ++c) { // TODO transform
    indices[c] = columnIndex(fptr, names[c]);
  }
  /* Read column metadata */
  long rows = rowCount(fptr);
  std::tuple<FitsIO::VecColumn<Ts>...> columns;
  Internal::ColumnLooper<sizeof...(Ts) - 1, Ts...>::readInfos(fptr, indices, columns, rows);
  int status = 0;
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
    Internal::ColumnLooper<sizeof...(Ts) - 1, Ts...>::readChunks(fptr, indices, columns, first, chunkRows);
  }
  return columns;
}

template <typename... Ts>
void writeColumns(fitsfile *fptr, const FitsIO::Column<Ts> &... columns) {
  int status = 0;
  /* Get chunk size */
  auto table = std::forward_as_tuple(columns...);
  auto rowCount = std::get<0>(table).rowCount();
  // FIXME assumes all columns have same height => move to write chunks
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  if (chunkRows == 0) {
    throw std::runtime_error("Cannot compute the optimal number of rows to be read at once");
  }
  /* Write column data */
  std::vector<long> indices { columnIndex(fptr, columns.info.name)... };
  for (long first = 1; first <= rowCount; first += chunkRows) {
    long last = first + chunkRows - 1;
    if (last > rowCount) {
      chunkRows = rowCount - first + 1;
    }
    Internal::ColumnLooper<sizeof...(Ts) - 1, Ts...>::writeChunks(fptr, indices, table, first, chunkRows);
  }
}

template <typename T>
void insertColumn(fitsfile *fptr, long index, const FitsIO::Column<T> &column) {
  auto name = toCharPtr(column.info.name);
  auto tform = toCharPtr(TypeCode<T>::tform(column.info.repeatCount));
  int status = 0;
  fits_insert_col(fptr, static_cast<int>(index), name.get(), tform.get(), &status);
  writeColumn(fptr, column);
}

template <typename... Ts>
void insertColumns(fitsfile *fptr, long index, const FitsIO::Column<Ts> &... columns) {
  auto names = CStrArray({ columns.info.name... });
  auto tforms = CStrArray({ TypeCode<Ts>::tform(columns.info.repeatCount)... });
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
