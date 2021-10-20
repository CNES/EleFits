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

#if defined(_ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL) || defined(CHECK_QUALITY)

  #include "EleCfitsioWrapper/BintableWrapper.h"
  #include "EleCfitsioWrapper/ErrorWrapper.h"
  #include "EleFitsData/FitsError.h"
  #include "ElementsKernel/Unused.h"

  #include <algorithm> // transform

namespace Euclid {
namespace Cfitsio {
namespace BintableIo {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Read metadata and allocate data.
 */
template <typename T>
void readColumnInfoImpl(fitsfile* fptr, long index, Fits::VecColumn<T>& column, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void readColumnInfoImpl<std::string>(fitsfile* fptr, long index, Fits::VecColumn<std::string>& column, long rowCount);

template <typename T>
void readColumnInfoImpl(fitsfile* fptr, long index, Fits::VecColumn<T>& column, long rowCount) {
  column = Fits::VecColumn<T>(
      readColumnInfo<T>(fptr, index),
      std::vector<std::decay_t<T>>(column.info().repeatCount * rowCount));
}

/**
 * @brief Read a column chunk.
 */
template <typename T>
void readColumnChunkImpl(fitsfile* fptr, long index, Fits::VecColumn<T>& column, long firstRow, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void readColumnChunkImpl<std::string>(
    fitsfile* fptr,
    long index,
    Fits::VecColumn<std::string>& column,
    long firstRow,
    long rowCount);

template <typename T>
void readColumnChunkImpl(fitsfile* fptr, long index, Fits::VecColumn<T>& column, long firstRow, long rowCount) {
  int status = 0;
  auto begin = column.data() + (firstRow - 1) * column.info().repeatCount;
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(),
      static_cast<int>(index),
      firstRow,
      1,
      rowCount * column.info().repeatCount,
      nullptr,
      begin,
      nullptr,
      &status);
  CfitsioError::mayThrow(
      status,
      fptr,
      "Cannot read column chunk: " + column.info().name + " (" + std::to_string(index - 1) + "); " + "rows: [" +
          std::to_string(firstRow - 1) + "-" + std::to_string(firstRow - 1 + rowCount - 1) + "-");
}

/**
 * @brief Write a column chunk.
 */
template <typename T>
void writeColumnChunkImpl(fitsfile* fptr, long index, const Fits::Column<T>& column, long firstRow, long rowCount);

/**
 * @brief String specialization.
 */
template <>
void writeColumnChunkImpl<std::string>(
    fitsfile* fptr,
    long index,
    const Fits::Column<std::string>& column,
    long firstRow,
    long rowCount);

template <typename T>
void writeColumnChunkImpl(fitsfile* fptr, long index, const Fits::Column<T>& column, long firstRow, long rowCount) {
  /* Allocate vector */
  const auto clipedRowCount = std::min(rowCount, column.rowCount() - firstRow + 1);
  const auto begin = column.data() + (firstRow - 1) * column.info().repeatCount;
  const auto size = clipedRowCount * column.info().repeatCount;
  const auto end = begin + size;
  std::vector<std::decay_t<T>> vec(begin, end);
  /* Write data */
  int status = 0;
  fits_write_col(fptr, TypeCode<T>::forBintable(), static_cast<int>(index), firstRow, 1, size, vec.data(), &status);
  CfitsioError::mayThrow(
      status,
      fptr,
      "Cannot write column chunk: " + column.info().name + " (" + std::to_string(index - 1) + "); " + "rows: [" +
          std::to_string(firstRow - 1) + "-" + std::to_string(firstRow - 1 + rowCount - 1) + "-");
}

/**
 * @brief Helper class to loop on a collection of columns.
 * @tparam i The index of the column the methods should be applied to:
 * should be initialized with `sizeof...(Ts) - 1`.
 * @details
 * This is a recursive implementation, decreasing from `sizeof...(Ts) - 1` to 0.
 * Calling a method with index `i` means applying the change to the `i`-th column,
 * and then calling the method with index `i - 1`.
 */
template <std::size_t i, typename... Ts>
struct ColumnLooperImpl {

  /**
   * @brief Read metadata and allocate data for each column
   */
  static void readInfos(
      fitsfile* fptr,
      const std::vector<long>& indices,
      std::tuple<Fits::VecColumn<Ts>...>& columns,
      long rowCount) {
    readColumnInfoImpl(fptr, indices[i], std::get<i>(columns), rowCount);
    ColumnLooperImpl<i - 1, Ts...>::readInfos(fptr, indices, columns, rowCount);
  }

  /**
   * @brief Read a chunk of each column
   */
  static void readChunks(
      fitsfile* fptr,
      const std::vector<long>& indices,
      std::tuple<Fits::VecColumn<Ts>...>& columns,
      long firstRow,
      long rowCount) {
    readColumnChunkImpl(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    ColumnLooperImpl<i - 1, Ts...>::readChunks(fptr, indices, columns, firstRow, rowCount);
  }

  /**
   * @brief Get the max number of rows of the columns.
   */
  static void maxRowCount(const std::tuple<const Fits::Column<Ts>&...>& columns, long& count = 0) {
    count = std::max(std::get<i>(columns).rowCount(), count);
    ColumnLooperImpl<i - 1, Ts...>::maxRowCount(columns, count);
  }

  /**
   * @brief Write a chunk of each column
   */
  static void writeChunks(
      fitsfile* fptr,
      const std::vector<long>& indices,
      std::tuple<const Fits::Column<Ts>&...> columns,
      long firstRow,
      long rowCount) {
    writeColumnChunkImpl(fptr, indices[i], std::get<i>(columns), firstRow, rowCount);
    ColumnLooperImpl<i - 1, Ts...>::writeChunks(fptr, indices, columns, firstRow, rowCount);
  }
};

/**
 * @brief Past-the-terminal case (do nothing).
 */
template <typename... Ts>
struct ColumnLooperImpl<std::size_t(-1), Ts...> {

  /** @brief Pass */
  static void readInfos(
      ELEMENTS_UNUSED fitsfile* fptr,
      ELEMENTS_UNUSED const std::vector<long>& indices,
      ELEMENTS_UNUSED std::tuple<Fits::VecColumn<Ts>...>& columns,
      ELEMENTS_UNUSED long rowCount) {}

  /** @brief Pass */
  static void readChunks(
      ELEMENTS_UNUSED fitsfile* fptr,
      ELEMENTS_UNUSED const std::vector<long>& indices,
      ELEMENTS_UNUSED std::tuple<Fits::VecColumn<Ts>...>& columns,
      ELEMENTS_UNUSED long firstRow,
      ELEMENTS_UNUSED long rowCount) {}

  /** @brief Pass */
  static void
  maxRowCount(ELEMENTS_UNUSED const std::tuple<const Fits::Column<Ts>&...>& columns, ELEMENTS_UNUSED long& count) {}

  /** @brief Pass */
  static void writeChunks(
      ELEMENTS_UNUSED fitsfile* fptr,
      ELEMENTS_UNUSED const std::vector<long>& indices,
      ELEMENTS_UNUSED std::tuple<const Fits::Column<Ts>&...> columns,
      ELEMENTS_UNUSED long firstRow,
      ELEMENTS_UNUSED long rowCount) {}
};

} // namespace Internal
/// @endcond

template <typename T>
Fits::ColumnInfo<T> readColumnInfo(fitsfile* fptr, long index) {
  int status = 0;
  char name[FLEN_VALUE];
  char unit[FLEN_VALUE];
  long repeatCount = 0;
  fits_get_bcolparms(
      fptr,
      index,
      name,
      unit,
      nullptr, // typechar
      &repeatCount,
      nullptr, // scale
      nullptr, // zero
      nullptr, // nulval
      nullptr, // tdisp
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read column info: #" + std::to_string(index - 1));
  return { name, unit, repeatCount };
}

template <typename T>
Fits::VecColumn<T> readColumn(fitsfile* fptr, long index) {
  const long rows = rowCount(fptr);
  Fits::VecColumn<T> column(readColumnInfo<T>(fptr, index), rows);
  readColumnSegment<T>(fptr, { 1, rows }, index, column);
  return column;
}

/// @cond INTERNAL
/**
 * @brief String specialization.
 */
template <>
void readColumnSegment<std::string>(
    fitsfile* fptr,
    const Fits::Segment& rows,
    long index,
    Fits::Column<std::string>& column);
/// @endcond

template <typename T>
void readColumnSegment(fitsfile* fptr, const Fits::Segment& rows, long index, Fits::Column<T>& column) {
  int status = 0;
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index),
      rows.front, // 1-based first row index
      1, // 1-based first element index
      rows.size() * column.info().repeatCount, // number of elements
      nullptr,
      column.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read column data: #" + std::to_string(index - 1));
}

template <typename T>
Fits::VecColumn<T> readColumn(fitsfile* fptr, const std::string& name) {
  return readColumn<T>(fptr, columnIndex(fptr, name));
}

/**
 * @brief String specialization.
 */
template <>
void writeColumn<std::string>(fitsfile* fptr, const Fits::Column<std::string>& column);

template <typename T>
void writeColumn(fitsfile* fptr, const Fits::Column<T>& column) {
  long index = columnIndex(fptr, column.info().name);
  const auto begin = column.data();
  const auto end = begin + column.elementCount();
  std::vector<std::decay_t<T>> nonconstData(begin, end); // We need a non-const data for CFitsIO
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
  CfitsioError::mayThrow(status, fptr, "Cannot write column data: " + column.info().name);
}

/**
 * @brief String specialization.
 */
template <>
void writeColumnSegment<std::string>(fitsfile* fptr, long firstRow, const Fits::Column<std::string>& column);

/**
 * @brief Const string specialization.
 */
template <>
void writeColumnSegment<const std::string>(fitsfile* fptr, long firstRow, const Fits::Column<const std::string>& column);

template <typename T>
void writeColumnSegment(fitsfile* fptr, long firstRow, const Fits::Column<T>& column) {
  long index = columnIndex(fptr, column.info().name);
  const auto begin = column.data();
  const auto end = begin + column.elementCount();
  std::vector<std::decay_t<T>> nonconstData(begin, end); // We need a non-const data for CFitsIO
  int status = 0;
  fits_write_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      firstRow, // firstrow (1-based)
      1, // firstelem (1-based)
      column.elementCount(), // nelements
      nonconstData.data(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write column data: " + column.info().name);
}

template <typename... Ts>
std::tuple<Fits::VecColumn<Ts>...> readColumns(fitsfile* fptr, const std::vector<long>& indices) {
  /* Read column metadata */
  const long rows = rowCount(fptr);
  std::tuple<Fits::VecColumn<Ts>...> columns;
  Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Ts...>::readInfos(fptr, indices, columns, rows);
  /* Get the buffer size */
  int status = 0;
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status);
  if (chunkRows == 0) {
    throw Fits::FitsError("Cannot compute the optimal number of rows to be read at once");
  }
  /* Read column data */
  for (long first = 1; first <= rows; first += chunkRows) {
    long last = first + chunkRows - 1;
    if (last > rows) {
      chunkRows = rows - first + 1;
    }
    Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Ts...>::readChunks(fptr, indices, columns, first, chunkRows);
  }
  return columns;
}

template <typename... Ts>
std::tuple<Fits::VecColumn<Ts>...> readColumns(fitsfile* fptr, const std::vector<std::string>& names) {
  /* List column indices */
  std::vector<long> indices(names.size());
  std::transform(names.cbegin(), names.cend(), indices.begin(), [&](const std::string& n) {
    return columnIndex(fptr, n);
  });
  return readColumns<Ts...>(fptr, indices);
}

template <typename... Ts>
void writeColumns(fitsfile* fptr, const Fits::Column<Ts>&... columns) {
  int status = 0;
  /* Get chunk size */
  const auto table = std::forward_as_tuple(columns...);
  long rows = 0;
  Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Ts...>::maxRowCount(table, rows);
  long chunkRows = 0;
  fits_get_rowsize(fptr, &chunkRows, &status); // Tested with other values, e.g. 1 and 10; less efficient
  if (chunkRows == 0) {
    throw Fits::FitsError("Cannot compute the optimal number of rows to be read at once");
  }
  /* Write column data */
  std::vector<long> indices { columnIndex(fptr, columns.info().name)... };
  for (long first = 1; first <= rows; first += chunkRows) {
    long last = first + chunkRows - 1;
    if (last > rows) {
      chunkRows = rows - first + 1;
    }
    Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Ts...>::writeChunks(fptr, indices, table, first, chunkRows);
  }
}

template <typename T>
void insertColumn(fitsfile* fptr, long index, const Fits::Column<T>& column) {
  auto name = toCharPtr(column.info().name);
  auto tform = toCharPtr(TypeCode<T>::tform(column.info().repeatCount));
  // FIXME write unit
  int status = 0;
  fits_insert_col(fptr, static_cast<int>(index), name.get(), tform.get(), &status);
  writeColumn(fptr, column);
}

template <typename... Ts>
void insertColumns(fitsfile* fptr, long index, const Fits::Column<Ts>&... columns) {
  auto names = CStrArray({ columns.info().name... });
  auto tforms = CStrArray({ TypeCode<Ts>::tform(columns.info().repeatCount)... });
  // FIXME write unit
  int status = 0;
  fits_insert_cols(fptr, static_cast<int>(index), sizeof...(Ts), names.data(), tforms.data(), &status);
  writeColumns(fptr, columns...);
}

template <typename T>
void appendColumn(fitsfile* fptr, const Fits::Column<T>& column) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insertColumn(fptr, ncols + 1, column);
}

template <typename... Ts>
void appendColumns(fitsfile* fptr, const Fits::Column<Ts>&... columns) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insertColumns(fptr, ncols + 1, columns...);
}

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid

#endif
