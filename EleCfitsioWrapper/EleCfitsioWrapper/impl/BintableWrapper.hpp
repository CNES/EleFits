// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h" // hasKeyword
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/FitsError.h"
#include "EleFitsUtils/StringUtils.h"

#include <algorithm> // transform

namespace Euclid {
namespace Cfitsio {
namespace BintableIo {

/// @cond
namespace Internal {

template <typename T>
void read_column_info_impl(
    fitsfile* fptr,
    long index,
    Fits::VecColumn<T>& column,
    long row_count) { // FIXME move outside of Internal
  column = Fits::VecColumn<T>(
      read_column_info<T>(fptr, index),
      std::vector<std::decay_t<T>>(column.info().elementCount() * row_count));
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
template <std::size_t i, typename... TColumns>
struct ColumnLooperImpl {

  /**
   * @brief Read metadata and allocate data for each column
   */
  static void
  read_infos(fitsfile* fptr, const std::vector<long>& indices, std::tuple<TColumns...>& columns, long row_count) {
    read_column_info_impl(fptr, indices[i], std::get<i>(columns), row_count);
    ColumnLooperImpl<i - 1, TColumns...>::read_infos(fptr, indices, columns, row_count);
  }

  /**
   * @brief Read a chunk of each column
   */
  static void read_chunks(
      fitsfile* fptr,
      const std::vector<long>& indices,
      std::tuple<TColumns...>& columns,
      long firstRow,
      long row_count) {
    auto data = &std::get<i>(columns)(firstRow - 1);
    const auto repeat_count = std::get<i>(columns).info().repeatCount();
    read_column_data(fptr, Fits::Segment::fromSize(firstRow, row_count), indices[i], repeat_count, data);
    ColumnLooperImpl<i - 1, TColumns...>::read_chunks(fptr, indices, columns, firstRow, row_count);
  }

  /**
   * @brief Get the max number of rows of the columns.
   */
  static void max_row_count(const std::tuple<const TColumns&...>& columns, long& count = 0) {
    count = std::max(std::get<i>(columns).rowCount(), count);
    ColumnLooperImpl<i - 1, TColumns...>::max_row_count(columns, count);
  }

  /**
   * @brief Write a chunk of each column
   */
  static void write_chunks(
      fitsfile* fptr,
      const std::vector<long>& indices,
      std::tuple<const TColumns&...> columns,
      long firstRow,
      long row_count) {
    const auto data = &std::get<i>(columns)(firstRow - 1);
    const auto repeat_count = std::get<i>(columns).info().repeatCount();
    write_column_data(fptr, Fits::Segment::fromSize(firstRow, row_count), indices[i], repeat_count, data);
    ColumnLooperImpl<i - 1, TColumns...>::write_chunks(fptr, indices, columns, firstRow, row_count);
  }
};

/**
 * @brief Past-the-terminal case (do nothing).
 */
template <typename... TColumns>
struct ColumnLooperImpl<std::size_t(-1), TColumns...> {

  /** @brief Pass */
  static void read_infos(fitsfile*, const std::vector<long>&, std::tuple<TColumns...>&, long) {}

  /** @brief Pass */
  static void read_chunks(fitsfile*, const std::vector<long>&, std::tuple<TColumns...>&, long, long) {}

  /** @brief Pass */
  static void max_row_count(const std::tuple<const TColumns&...>&, long&) {}

  /** @brief Pass */
  static void write_chunks(fitsfile*, const std::vector<long>&, std::tuple<const TColumns&...>, long, long) {}
};

} // namespace Internal
/// @endcond

template <typename T, long N> // USED
Fits::ColumnInfo<T, N> read_column_info(fitsfile* fptr, long index) {
  int status = 0;
  char name[FLEN_VALUE];
  char unit[FLEN_VALUE];
  long repeat_count = 0;
  fits_get_bcolparms(
      fptr,
      index,
      name,
      unit,
      nullptr, // typechar
      &repeat_count,
      nullptr, // scale
      nullptr, // zero
      nullptr, // nulval
      nullptr, // tdisp
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot read column info: #" + std::to_string(index - 1));
  Fits::ColumnInfo<T, N> info(name, unit, repeat_count);
  read_column_dim(fptr, index, info.shape);
  return info;
}

template <>
void read_column_dim(fitsfile* fptr, long index, Fits::Position<-1>& shape);

template <long N>
void read_column_dim(fitsfile* fptr, long index, Fits::Position<N>& shape) {
  if (not HeaderIo::has_keyword(fptr, std::string("TDIM") + std::to_string(index))) {
    return;
  }
  int status = 0;
  int naxis = 0;
  fits_read_tdim(fptr, static_cast<int>(index), N, &naxis, shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot read column dimension: #" + std::to_string(index - 1));
}

template <typename T, long N>
Fits::VecColumn<T, N> read_column(fitsfile* fptr, long index) {
  const long rows = row_count(fptr);
  Fits::VecColumn<T, N> column(read_column_info<T>(fptr, index), rows);
  read_column_data(fptr, {1, rows}, index, column.info().repeatCount(), column.data());
  return column;
}

template <typename TColumn>
void read_column_segment(fitsfile* fptr, const Fits::Segment& rows, long index, TColumn& column) {
  read_column_data(fptr, rows, index, column.info().repeatCount(), column.data());
}

template <typename T, long N>
Fits::VecColumn<T, N> read_column(fitsfile* fptr, const std::string& name) {
  return read_column<T, N>(fptr, column_index(fptr, name));
}

template <typename TColumn>
void write_column(fitsfile* fptr, const TColumn& column) {
  write_column_segment(fptr, 1, column);
}

template <long N>
void write_column_dim(fitsfile* fptr, long index, const Fits::Position<N>& shape) {
  if (shape.size() == 1) {
    return;
  }
  int status = 0;
  auto nonconst_shape = shape.container();
  fits_write_tdim(fptr, static_cast<int>(index), shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot write column dimension: #" + std::to_string(index - 1));
}

template <typename... TInfos>
void write_column_dims(fitsfile* fptr, long index, const TInfos&... infos) { // FIXME pass shape
  auto i = index;
  (void)fptr;
  using mock_unpack = long[];
  (void)mock_unpack {0L, (write_column_dim(fptr, i, infos.shape), ++i)...};
}

template <typename TColumn>
void write_column_segment(fitsfile* fptr, long firstRow, const TColumn& column) {
  const auto index = column_index(fptr, column.info().name);
  write_column_data(
      fptr,
      Fits::Segment::fromSize(firstRow, column.rowCount()),
      index,
      column.info().repeatCount(),
      column.data());
}

template <typename... Ts> // FIXME Ns
std::tuple<Fits::VecColumn<Ts, 1>...> read_columns(fitsfile* fptr, const std::vector<long>& indices) {
  /* Read column metadata */
  const long rows = row_count(fptr);
  std::tuple<Fits::VecColumn<Ts, 1>...> columns;
  Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Fits::VecColumn<Ts, 1>...>::read_infos(fptr, indices, columns, rows);
  /* Get the buffer size */
  int status = 0;
  long chunk_row_count = 0;
  fits_get_rowsize(fptr, &chunk_row_count, &status);
  if (chunk_row_count == 0) {
    throw Fits::FitsError("Cannot compute the optimal number of rows to be read at once");
  }
  /* Read column data */
  for (long first = 1; first <= rows; first += chunk_row_count) {
    long last = first + chunk_row_count - 1;
    if (last > rows) {
      chunk_row_count = rows - first + 1;
    }
    Internal::ColumnLooperImpl<sizeof...(Ts) - 1, Fits::VecColumn<Ts, 1>...>::read_chunks(
        fptr,
        indices,
        columns,
        first,
        chunk_row_count);
  }
  return columns;
}

template <typename... Ts> // FIXME Ns
std::tuple<Fits::VecColumn<Ts, 1>...> read_columns(fitsfile* fptr, const std::vector<std::string>& names) {
  /* List column indices */
  std::vector<long> indices(names.size());
  std::transform(names.cbegin(), names.cend(), indices.begin(), [&](const std::string& n) {
    return column_index(fptr, n);
  });
  return read_columns<Ts...>(fptr, indices);
}

template <typename... TColumns>
void write_columns(fitsfile* fptr, const TColumns&... columns) {
  int status = 0;
  /* Get chunk size */
  const auto table = std::forward_as_tuple(columns...);
  long rows = 0;
  Internal::ColumnLooperImpl<sizeof...(TColumns) - 1, TColumns...>::max_row_count(table, rows);
  long chunk_row_count = 0;
  fits_get_rowsize(fptr, &chunk_row_count, &status); // Tested with other values, e.g. 1 and 10; less efficient
  if (chunk_row_count == 0) {
    throw Fits::FitsError("Cannot compute the optimal number of rows to be read at once");
  }
  /* Write column data */
  std::vector<long> indices {column_index(fptr, columns.info().name)...};
  for (long first = 1; first <= rows; first += chunk_row_count) {
    long last = first + chunk_row_count - 1;
    if (last > rows) {
      chunk_row_count = rows - first + 1;
    }
    Internal::ColumnLooperImpl<sizeof...(TColumns) - 1, TColumns...>::write_chunks(
        fptr,
        indices,
        table,
        first,
        chunk_row_count);
  }
}

template <typename TColumn>
void insert_columnn(fitsfile* fptr, long index, const TColumn& column) {
  const auto& info = column.info();
  auto name = Fits::String::toCharPtr(info.name);
  auto tform = Fits::String::toCharPtr(TypeCode<typename TColumn::Value>::tform(info.repeatCount()));
  // FIXME write unit
  int status = 0;
  fits_insert_col(fptr, static_cast<int>(index), name.get(), tform.get(), &status);
  write_column_dim(fptr, index, info.shape);
  write_column(fptr, column);
}

template <typename... TColumns>
void insert_columns(fitsfile* fptr, long index, const TColumns&... columns) {
  auto names = Fits::String::CStrArray({columns.info().name...});
  auto tforms = Fits::String::CStrArray({TypeCode<typename TColumns::Value>::tform(columns.info().repeatCount())...});
  // FIXME write unit
  int status = 0;
  fits_insert_cols(fptr, static_cast<int>(index), sizeof...(TColumns), names.data(), tforms.data(), &status);
  write_column_dims(fptr, index, columns.info()...);
  write_columns(fptr, columns...);
}

template <typename TColumn>
void append_column(fitsfile* fptr, const TColumn& column) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insert_columnn(fptr, ncols + 1, column);
}

template <typename... TColumns>
void append_columns(fitsfile* fptr, const TColumns&... columns) {
  int ncols = 0;
  int status = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  insert_columns(fptr, ncols + 1, columns...);
}

template <typename T>
void read_column_data(fitsfile* fptr, const Fits::Segment& rows, long index, long repeat_count, T* data) {
  int status = 0;
  const auto size = rows.size() * repeat_count;
  fits_read_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index),
      rows.front, // 1-based first row index
      1, // 1-based first element index
      size, // number of elements
      nullptr,
      data,
      nullptr,
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot read column data: #" + std::to_string(index - 1));
}

template <typename T>
void write_column_data(fitsfile* fptr, const Fits::Segment& rows, long index, long repeat_count, const T* data) {
  int status = 0;
  const auto size = rows.size() * repeat_count;
  std::vector<T> nonconst_data(data, data + size); // We need a non-const data for CFITSIO
  fits_write_col(
      fptr,
      TypeCode<T>::forBintable(), // datatype
      static_cast<int>(index), // colnum
      rows.front, // firstrow (1-based)
      1, // firstelem (1-based)
      size, // nelements
      nonconst_data.data(),
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot write column data: #" + std::to_string(index - 1));
}

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid

#endif
