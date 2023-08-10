// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_BINTABLECOLUMNS_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h" // TODO rm when implementation of init(Seq) is in BintableWrapper
#include "EleFits/BintableColumns.h"

namespace Euclid {
namespace Fits {

// Implementation rules for overloads
//
// - Flow should go from names to indices: never call readName() internally, and call readIndex() once;
// - Variadic methods should call TSeq methods through std::forward_as_tuple because TSec is more generic;
// - TSeq&& should be forwarded as std::forward<TSeq>();
// - Duplication should be minimal: when there are two ways with unavoidable duplication, choose the minimalist option.
//
// Exceptions to these rules must be explicitely justified.

// readInfo

template <typename T, long N>
ColumnInfo<T, N> BintableColumns::readInfo(ColumnKey key) const {
  return Cfitsio::BintableIo::read_column_info<T, N>(m_fptr, key.index(*this) + 1); // 1-based
}

// read

template <typename T, long N>
VecColumn<T, N> BintableColumns::read(ColumnKey key) const {
  return readSegment<T, N>({0, -1}, key);
}

// readTo

template <typename TColumn>
void BintableColumns::readTo(TColumn& column) const {
  readTo(column.info().name, column);
}

template <typename TColumn>
void BintableColumns::readTo(ColumnKey key, TColumn& column) const {
  readSegmentTo({0, -1}, key, column);
}

// readSegment

template <typename T, long N>
VecColumn<T, N> BintableColumns::readSegment(const Segment& rows, ColumnKey key) const {
  const auto index = key.index(*this);
  auto resolved_rows = rows;
  if (rows.back == -1) {
    resolved_rows.back = readRowCount() - 1;
  }
  VecColumn<T, N> column(readInfo<T, N>(std::move(key)), resolved_rows.size());
  readSegmentTo(resolved_rows, index, column);
  return column;
}

// readSegmentTo

template <typename TColumn>
void BintableColumns::readSegmentTo(FileMemSegments rows, TColumn& column) const {
  readSegmentTo(std::move(rows), column.info().name, column);
}

template <typename TColumn>
void BintableColumns::readSegmentTo(FileMemSegments rows, ColumnKey key, TColumn& column) const {
  m_touch();
  rows.resolve(readRowCount() - 1, column.row_count() - 1);
  auto slice = column.slice(rows.memory()); // TODO do we need a temporary variable?
  Cfitsio::BintableIo::read_column_data(
      m_fptr,
      Segment {rows.file().front + 1, rows.file().back + 1}, // TODO operator+
      key.index(*this) + 1, // 1-based
      column.info().repeat_count(),
      &column(rows.memory().front, 0));
}

// readSeq

template <typename TKey, typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const TypedKey<Ts, TKey>&... keys) const {
  const auto row_count = readRowCount();
  std::tuple<VecColumn<Ts>...> res {VecColumn<Ts>(readInfo<Ts>(keys.key), row_count)...};
  readSeqTo({ColumnKey(keys.key)...}, res);
  return res;
}

template <typename T, long N>
std::vector<VecColumn<T, N>> BintableColumns::readSeq(std::vector<ColumnKey> keys) const {
  const auto row_count = readRowCount();
  std::vector<VecColumn<T, N>> res(keys.size());
  std::transform(keys.begin(), keys.end(), res.begin(), [&](ColumnKey& k) {
    return VecColumn<T, N>(readInfo<T>(k.index(*this)), row_count);
  });
  readSeqTo(std::move(keys), res);
  return res;
}

// readSeqTo

template <typename TSeq>
void BintableColumns::readSeqTo(TSeq&& columns) const {
  auto keys = seqTransform<std::vector<ColumnKey>>(columns, [&](const auto& c) {
    return c.info().name;
  });
  readSeqTo(std::move(keys), std::forward<TSeq>(columns));
}

template <typename... TColumns>
void BintableColumns::readSeqTo(TColumns&... columns) const {
  readSeqTo(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSeqTo(std::vector<ColumnKey> keys, TSeq&& columns) const {
  readSegmentSeqTo(0, std::move(keys), std::forward<TSeq>(columns));
}

template <typename... TColumns>
void BintableColumns::readSeqTo(std::vector<ColumnKey> keys, TColumns&... columns) const {
  readSeqTo(std::move(keys), std::forward_as_tuple(columns...));
}

// readSegmentSeq

template <typename TKey, typename... Ts>
std::tuple<VecColumn<Ts, 1>...> BintableColumns::readSegmentSeq(Segment rows, const TypedKey<Ts, TKey>&... keys) const {
  if (rows.back == -1) {
    rows.back = readRowCount() - 1;
  }
  auto columns = std::make_tuple(VecColumn<Ts, 1>(readInfo<Ts, 1>(keys.key), rows.size())...);
  readSegmentSeqTo(rows, {ColumnKey(keys.key)...}, columns);
  return columns;
}

template <typename T, long N>
std::vector<VecColumn<T, N>> BintableColumns::readSegmentSeq(Segment rows, std::vector<ColumnKey> keys) const {
  if (rows.back == -1) {
    rows.back = readRowCount() - 1;
  }
  std::vector<VecColumn<T, N>> columns;
  columns.reserve(keys.size());
  for (const auto& k : keys) {
    columns.emplace_back(readInfo<T>(k), rows.size()); // TODO std::transform?
  }
  readSegmentSeqTo(rows, keys, columns);
  return columns;
}

// readSegmentSeqTo

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(FileMemSegments rows, TSeq&& columns) const {
  auto keys = seqTransform<std::vector<ColumnKey>>(columns, [&](const auto& c) {
    return c.info().name;
  });
  readSegmentSeqTo(std::move(rows), keys, std::forward<TSeq>(columns));
}

template <typename... TColumns>
void BintableColumns::readSegmentSeqTo(FileMemSegments rows, TColumns&... columns) const {
  readSegmentSeqTo(std::move(rows), {ColumnKey(columns.info().name)...}, columns...);
  // Could forward_as_tuple but would be 1 more indirection for the same amount of lines
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(FileMemSegments rows, std::vector<ColumnKey> keys, TSeq&& columns) const {
  const auto buffer_size = readBufferRowCount();
  const long row_count = columnsRowCount(std::forward<TSeq>(columns));
  rows.resolve(readRowCount() - 1, row_count - 1);
  const long last_mem_row = rows.memory().back;
  for (Segment file = Segment::fromSize(rows.file().front, buffer_size), // FIXME use a FileMemSegments
       mem = Segment::fromSize(rows.memory().front, buffer_size);
       mem.front <= last_mem_row;
       file += buffer_size, mem += buffer_size) {
    if (mem.back > last_mem_row) {
      mem.back = last_mem_row;
    }
    auto it = keys.begin();
    seqForeach(std::forward<TSeq>(columns), [&](auto& c) {
      readSegmentTo({file.front, mem}, it->index(*this), c);
      ++it;
    });
  }
}

template <typename... TColumns>
void BintableColumns::readSegmentSeqTo(FileMemSegments rows, std::vector<ColumnKey> keys, TColumns&... columns) const {
  readSegmentSeqTo(std::move(rows), std::move(keys), std::forward_as_tuple(columns...));
}

// write

template <typename TColumn>
void BintableColumns::write(const TColumn& column) const {
  writeSegment(0, column);
}

// init

template <typename TInfo>
void BintableColumns::init(const TInfo& info, long index) const {
  m_edit();
  auto name = Fits::String::toCharPtr(info.name);
  auto tform = Fits::String::toCharPtr(Cfitsio::TypeCode<typename TInfo::Value>::tform(info.repeat_count()));
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

// writeSegment

template <typename TColumn>
void BintableColumns::writeSegment(FileMemSegments rows, const TColumn& column) const {
  m_edit();
  rows.resolve(readRowCount() - 1, column.row_count() - 1);
  const auto index = readIndex(column.info().name); // FIXME avoid?
  Cfitsio::BintableIo::write_column_data(
      m_fptr,
      rows.file() + 1,
      index + 1,
      column.info().repeat_count(),
      &column(rows.memory().front, 0));
}

// writeSeq

template <typename TSeq>
void BintableColumns::writeSeq(TSeq&& columns) const {
  writeSegmentSeq(0, std::forward<TSeq>(columns));
}

template <typename... TColumns>
void BintableColumns::writeSeq(const TColumns&... columns) const {
  writeSeq(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::initSeq(long index, TSeq&& infos) const {
  m_edit();
  const auto nameVec = seqTransform<std::vector<std::string>>(infos, [&](const auto& info) {
    return info.name;
  });
  String::CStrArray names(nameVec);
  const auto tformVec = seqTransform<std::vector<std::string>>(infos, [&](const auto& info) {
    using Value = typename std::decay_t<decltype(info)>::Value;
    return Cfitsio::TypeCode<std::decay_t<Value>>::tform(info.repeat_count());
  });
  String::CStrArray tforms(tformVec);
  int status = 0;
  int cfitsio_index = index == -1 ? Cfitsio::BintableIo::column_count(m_fptr) + 1 : index + 1;
  fits_insert_cols(m_fptr, cfitsio_index, names.size(), names.data(), tforms.data(), &status);
  // TODO to Cfitsio
  long i = cfitsio_index;
  seqForeach(std::forward<TSeq>(infos), [&](const auto& info) { // FIXME duplication
    if (info.unit != "") {
      const Record<std::string> record {"TUNIT" + std::to_string(i), info.unit, "", "physical unit of field"};
      Cfitsio::HeaderIo::update_record(m_fptr, record);
    }
    ++i;
  });
  // TODO to Cfitsio
}

template <typename... TInfos>
void BintableColumns::initSeq(long index, const TInfos&... infos) const {
  initSeq(index, std::forward_as_tuple(infos...));
}

// writeSegmentSeq

template <typename TSeq>
void BintableColumns::writeSegmentSeq(FileMemSegments rows, TSeq&& columns) const {
  const auto row_count = columnsRowCount(std::forward<TSeq>(columns));
  rows.resolve(readRowCount() - 1, row_count - 1);
  const long last_mem_row = rows.memory().back;
  const auto buffer_size = readBufferRowCount();
  for (auto mem = Segment::fromSize(rows.memory().front, buffer_size), // FIXME use a FileMemSegments
       file = Segment::fromSize(rows.file().front, buffer_size);
       mem.front <= last_mem_row;
       mem += buffer_size, file += buffer_size) {
    if (mem.back > last_mem_row) {
      mem.back = last_mem_row;
    }
    seqForeach(std::forward<TSeq>(columns), [&](const auto& c) {
      writeSegment({file.front, mem}, c); // FIXME don't recalculate index
    });
  }
}

template <typename... TColumns>
void BintableColumns::writeSegmentSeq(FileMemSegments rows, const TColumns&... columns) const {
  writeSegmentSeq(std::move(rows), std::forward_as_tuple(columns...));
}

template <typename TSeq>
long columnsRowCount(TSeq&& columns) {
  long rows = -1;
  seqForeach(std::forward<TSeq>(columns), [&](const auto& c) {
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
} // namespace Euclid

#endif
