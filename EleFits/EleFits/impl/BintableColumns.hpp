// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_BINTABLECOLUMNS_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h" // TODO rm when implementation of init(Seq) is in BintableWrapper
#include "EleFits/BintableColumns.h"
#include "Linx/Base/SeqUtils.h" // seq_foreach

namespace Fits {

// Implementation rules for overloads
//
// - Flow should go from names to indices: never call read_name() internally, and call read_index() once;
// - Variadic methods should call TSeq methods through std::forward_as_tuple because TSec is more generic;
// - TSeq&& should be forwarded with `LINX_FORWARD()`;
// - Duplication should be minimal: when there are two ways with unavoidable duplication, choose the minimalist option.
//
// Exceptions to these rules must be explicitely justified.

// read_info

template <typename T, Linx::Index N>
ColumnInfo<T, N> BintableColumns::read_info(ColumnKey key) const
{
  return Cfitsio::BintableIo::read_column_info<T, N>(m_fptr, key.index(*this) + 1); // 1-based
}

// read

template <typename T, Linx::Index N>
VecColumn<T, N> BintableColumns::read(ColumnKey key) const
{
  return read_segment<T, N>({0, -1}, key);
}

// read_to

template <typename TColumn>
void BintableColumns::read_to(TColumn& column) const
{
  read_to(column.info().name, column);
}

template <typename TColumn>
void BintableColumns::read_to(ColumnKey key, TColumn& column) const
{
  read_segment_to({0, -1}, key, column);
}

// read_segment

template <typename T, Linx::Index N>
VecColumn<T, N> BintableColumns::read_segment(const Segment& rows, ColumnKey key) const
{
  const auto index = key.index(*this);
  auto resolved_rows = rows;
  if (rows.back == -1) {
    resolved_rows.back = read_row_count() - 1;
  }
  VecColumn<T, N> column(read_info<T, N>(std::move(key)), resolved_rows.size());
  read_segment_to(resolved_rows, index, column);
  return column;
}

// read_segment_to

template <typename TColumn>
void BintableColumns::read_segment_to(FileMemSegments rows, TColumn& column) const
{
  read_segment_to(std::move(rows), column.info().name, column);
}

template <typename TColumn>
void BintableColumns::read_segment_to(FileMemSegments rows, ColumnKey key, TColumn& column) const
{
  m_touch();
  rows.resolve(read_row_count() - 1, column.row_count() - 1);
  auto slice = column.slice(rows.memory()); // TODO do we need a temporary variable?
  Cfitsio::BintableIo::read_column_data(
      m_fptr,
      Segment {rows.file().front + 1, rows.file().back + 1}, // TODO operator+
      key.index(*this) + 1, // 1-based
      column.info().repeat_count(),
      &column(rows.memory().front, 0));
}

// read_n

template <typename TKey, typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::read_n(const TypedKey<Ts, TKey>&... keys) const
{
  const auto row_count = read_row_count();
  std::tuple<VecColumn<Ts>...> res {VecColumn<Ts>(read_info<Ts>(keys.key), row_count)...};
  read_n_to({ColumnKey(keys.key)...}, res);
  return res;
}

template <typename T, Linx::Index N>
std::vector<VecColumn<T, N>> BintableColumns::read_n(std::vector<ColumnKey> keys) const
{
  const auto row_count = read_row_count();
  std::vector<VecColumn<T, N>> res(keys.size());
  std::transform(keys.begin(), keys.end(), res.begin(), [&](ColumnKey& k) {
    return VecColumn<T, N>(read_info<T>(k.index(*this)), row_count);
  });
  read_n_to(std::move(keys), res);
  return res;
}

// read_n_to

template <typename TSeq>
void BintableColumns::read_n_to(TSeq&& columns) const
{
  auto keys = Linx::seq_transform<std::vector<ColumnKey>>(columns, [&](const auto& c) {
    return c.info().name;
  });
  read_n_to(std::move(keys), LINX_FORWARD(columns));
}

template <typename... TColumns>
void BintableColumns::read_n_to(TColumns&... columns) const
{
  read_n_to(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::read_n_to(std::vector<ColumnKey> keys, TSeq&& columns) const
{
  read_n_segments_to(0, std::move(keys), LINX_FORWARD(columns));
}

template <typename... TColumns>
void BintableColumns::read_n_to(std::vector<ColumnKey> keys, TColumns&... columns) const
{
  read_n_to(std::move(keys), std::forward_as_tuple(columns...));
}

// read_n_segments

template <typename TKey, typename... Ts>
std::tuple<VecColumn<Ts, 1>...> BintableColumns::read_n_segments(Segment rows, const TypedKey<Ts, TKey>&... keys) const
{
  if (rows.back == -1) {
    rows.back = read_row_count() - 1;
  }
  auto columns = std::make_tuple(VecColumn<Ts, 1>(read_info<Ts, 1>(keys.key), rows.size())...);
  read_n_segments_to(rows, {ColumnKey(keys.key)...}, columns);
  return columns;
}

template <typename T, Linx::Index N>
std::vector<VecColumn<T, N>> BintableColumns::read_n_segments(Segment rows, std::vector<ColumnKey> keys) const
{
  if (rows.back == -1) {
    rows.back = read_row_count() - 1;
  }
  std::vector<VecColumn<T, N>> columns;
  columns.reserve(keys.size());
  for (const auto& k : keys) {
    columns.emplace_back(read_info<T>(k), rows.size()); // TODO std::transform?
  }
  read_n_segments_to(rows, keys, columns);
  return columns;
}

// read_n_segments_to

template <typename TSeq>
void BintableColumns::read_n_segments_to(FileMemSegments rows, TSeq&& columns) const
{
  auto keys = Linx::seq_transform<std::vector<ColumnKey>>(columns, [&](const auto& c) {
    return c.info().name;
  });
  read_n_segments_to(std::move(rows), keys, LINX_FORWARD(columns));
}

template <typename... TColumns>
void BintableColumns::read_n_segments_to(FileMemSegments rows, TColumns&... columns) const
{
  read_n_segments_to(std::move(rows), {ColumnKey(columns.info().name)...}, columns...);
  // Could forward_as_tuple but would be 1 more indirection for the same amount of lines
}

template <typename TSeq>
void BintableColumns::read_n_segments_to(FileMemSegments rows, std::vector<ColumnKey> keys, TSeq&& columns) const
{
  const auto buffer_size = read_buffer_row_count();
  const Linx::Index row_count = columns_row_count(LINX_FORWARD(columns));
  rows.resolve(read_row_count() - 1, row_count - 1);
  const Linx::Index last_mem_row = rows.memory().back;
  for (Segment file = Segment::fromSize(rows.file().front, buffer_size), // FIXME use a FileMemSegments
       mem = Segment::fromSize(rows.memory().front, buffer_size);
       mem.front <= last_mem_row;
       file += buffer_size, mem += buffer_size) {
    if (mem.back > last_mem_row) {
      mem.back = last_mem_row;
    }
    auto it = keys.begin();
    Linx::seq_foreach(LINX_FORWARD(columns), [&](auto& c) {
      read_segment_to({file.front, mem}, it->index(*this), c);
      ++it;
    });
  }
}

template <typename... TColumns>
void BintableColumns::read_n_segments_to(FileMemSegments rows, std::vector<ColumnKey> keys, TColumns&... columns) const
{
  read_n_segments_to(std::move(rows), std::move(keys), std::forward_as_tuple(columns...));
}

// write

template <typename TColumn>
void BintableColumns::write(const TColumn& column) const
{
  write_segment(0, column);
}

// init

template <typename TInfo>
void BintableColumns::insert_null(Linx::Index index, const TInfo& info) const
{
  m_edit();
  auto name = Fits::String::to_char_ptr(info.name);
  auto tform = Fits::String::to_char_ptr(Cfitsio::TypeCode<typename TInfo::Value>::tform(info.repeat_count()));
  int status = 0;
  int cfitsio_index = index == -1 ? Cfitsio::BintableIo::column_count(m_fptr) + 1 : index + 1;
  fits_insert_col(m_fptr, cfitsio_index, name.get(), tform.get(), &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot init new column: #" + std::to_string(index));
  if (info.unit != "") {
    const Record<std::string> record {"TUNIT" + std::to_string(cfitsio_index), info.unit, "", "physical unit of field"};
    Cfitsio::HeaderIo::update_record(m_fptr, record);
  }
  // TODO to Cfitsio
}

// write_segment

template <typename TColumn>
void BintableColumns::write_segment(FileMemSegments rows, const TColumn& column) const
{
  m_edit();
  rows.resolve(read_row_count() - 1, column.row_count() - 1);
  const auto index = read_index(column.info().name); // FIXME avoid?
  Cfitsio::BintableIo::write_column_data(
      m_fptr,
      rows.file() + 1,
      index + 1,
      column.info().repeat_count(),
      &column(rows.memory().front, 0));
}

// write_n

template <typename TSeq>
void BintableColumns::write_n(TSeq&& columns) const
{
  write_n_segments(0, LINX_FORWARD(columns));
}

template <typename... TColumns>
void BintableColumns::write_n(const TColumns&... columns) const
{
  write_n(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::insert_n_null(Linx::Index index, TSeq&& infos) const
{
  m_edit();
  const auto name_vec = Linx::seq_transform<std::vector<std::string>>(infos, [&](const auto& info) {
    return info.name;
  });
  String::CStrArray names(name_vec);
  const auto tform_vec = Linx::seq_transform<std::vector<std::string>>(infos, [&](const auto& info) {
    using Value = typename std::decay_t<decltype(info)>::Value;
    return Cfitsio::TypeCode<std::decay_t<Value>>::tform(info.repeat_count());
  });
  String::CStrArray tforms(tform_vec);
  int status = 0;
  int cfitsio_index = index == -1 ? Cfitsio::BintableIo::column_count(m_fptr) + 1 : index + 1;
  fits_insert_cols(m_fptr, cfitsio_index, names.size(), names.data(), tforms.data(), &status);
  // TODO to Cfitsio
  Linx::Index i = cfitsio_index;
  Linx::seq_foreach(LINX_FORWARD(infos), [&](const auto& info) { // FIXME duplication
    if (info.unit != "") {
      const Record<std::string> record {"TUNIT" + std::to_string(i), info.unit, "", "physical unit of field"};
      Cfitsio::HeaderIo::update_record(m_fptr, record);
    }
    ++i;
  });
  // TODO to Cfitsio
}

template <typename... TInfos>
void BintableColumns::insert_n_null(Linx::Index index, const TInfos&... infos) const
{
  insert_n_null(index, std::forward_as_tuple(infos...));
}

// write_n_segments

template <typename TSeq>
void BintableColumns::write_n_segments(FileMemSegments rows, TSeq&& columns) const
{
  const auto row_count = columns_row_count(LINX_FORWARD(columns));
  rows.resolve(read_row_count() - 1, row_count - 1);
  const Linx::Index last_mem_row = rows.memory().back;
  const auto buffer_size = read_buffer_row_count();
  for (auto mem = Segment::fromSize(rows.memory().front, buffer_size), // FIXME use a FileMemSegments
       file = Segment::fromSize(rows.file().front, buffer_size);
       mem.front <= last_mem_row;
       mem += buffer_size, file += buffer_size) {
    if (mem.back > last_mem_row) {
      mem.back = last_mem_row;
    }
    Linx::seq_foreach(LINX_FORWARD(columns), [&](const auto& c) {
      write_segment({file.front, mem}, c); // FIXME don't recalculate index
    });
  }
}

template <typename... TColumns>
void BintableColumns::write_n_segments(FileMemSegments rows, const TColumns&... columns) const
{
  write_n_segments(std::move(rows), std::forward_as_tuple(columns...));
}

template <typename TSeq>
Linx::Index columns_row_count(TSeq&& columns)
{
  Linx::Index rows = -1;
  Linx::seq_foreach(LINX_FORWARD(columns), [&](const auto& c) {
    const auto c_rows = c.row_count();
    if (rows == -1) {
      rows = c_rows;
    } else if (c_rows != rows) {
      throw FitsError("Columns do not have the same number of rows."); // FIXME clean
    }
  });
  return rows;
}

} // namespace Fits

#endif
