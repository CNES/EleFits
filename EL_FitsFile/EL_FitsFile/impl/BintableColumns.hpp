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

#if defined(_EL_FITSFILE_BINTABLECOLUMNS_IMPL) || defined(CHECK_QUALITY)

  #include "EL_CfitsioWrapper/BintableWrapper.h"
  #include "EL_CfitsioWrapper/HeaderWrapper.h" // TODO rm when implementation of init(Seq) is in BintableWrapper
  #include "EL_FitsFile/BintableColumns.h"

namespace Euclid {
namespace FitsIO {

// Implementation rules for overloads
//
// - Flow should go from names to indices: never call readName() internally, and call readIndex() once;
// - Variadic methods should call TSeq methods through std::forward_as_tuple because TSec is more generic;
// - TSeq&& should be forwarded as std::forward<TSeq>();
// - Duplication should be minimal: when there are two ways with unavoidable duplication, choose the minimalist option.
//
// Exceptions to these rules must be explicitely justified.

// readInfo

template <typename T>
ColumnInfo<T> BintableColumns::readInfo(const std::string& name) const {
  return readInfo<T>(readIndex(name));
}

template <typename T>
ColumnInfo<T> BintableColumns::readInfo(long index) const {
  return Cfitsio::BintableIo::readColumnInfo<T>(m_fptr, index + 1); // 1-based
}

// read

template <typename T>
VecColumn<T> BintableColumns::read(const std::string& name) const {
  return read<T>(readIndex(name));
}

template <typename T>
VecColumn<T> BintableColumns::read(long index) const {
  return readSegment<T>({ 0, readRowCount() - 1 }, index);
}

// readTo

template <typename T>
void BintableColumns::readTo(Column<T>& column) const {
  readTo<T>(column.info.name, column);
}

template <typename T>
void BintableColumns::readTo(const std::string& name, Column<T>& column) const {
  readTo<T>(readIndex(name), column);
}

template <typename T>
void BintableColumns::readTo(long index, Column<T>& column) const {
  readSegmentTo<T>({ 0, readRowCount() - 1 }, column);
}

// readSegment

template <typename T>
VecColumn<T> BintableColumns::readSegment(const Segment& rows, const std::string& name) const {
  return readSegment<T>(rows, readIndex(name));
}

template <typename T>
VecColumn<T> BintableColumns::readSegment(const Segment& rows, long index) const {
  VecColumn<T> column(readInfo<T>(index), rows.size());
  readSegmentTo<T>(rows.first, index, column);
  return column;
}

// readSegmentTo

template <typename T>
void BintableColumns::readSegmentTo(long firstRow, Column<T>& column) const {
  readSegmentTo<T>(firstRow, column.info.name, column);
}

template <typename T>
void BintableColumns::readSegmentTo(long firstRow, const std::string& name, Column<T>& column) const {
  readSegmentTo<T>(firstRow, readIndex(name), column);
}

template <typename T>
void BintableColumns::readSegmentTo(long firstRow, long index, Column<T>& column) const {
  m_touch();
  const auto cfitsioFirstRow = firstRow == -1 ? readRowCount() + 1 : firstRow + 1; // 1-based
  const auto cfitsioRows = Segment::fromSize(cfitsioFirstRow, column.rowCount());
  Cfitsio::BintableIo::readColumnSegment<T>(m_fptr, cfitsioRows, index + 1, column);
}

// readSeq

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Named<Ts>&... names) const {
  return readSeq(Indexed<Ts>(readIndex(names.name))...);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Indexed<Ts>&... indices) const {
  m_touch();
  const auto rowCount = readRowCount();
  std::tuple<VecColumn<Ts>...> res { VecColumn<Ts>(readInfo<Ts>(indices), std::vector<Ts>(rowCount))... };
  readSeqTo({ indices... }, res);
  return res;
}

// readSeqTo

template <typename TSeq>
void BintableColumns::readSeqTo(TSeq&& columns) const {
  const auto names = seqTransform<std::vector<std::string>>(std::forward<TSeq>(columns), [&](const auto& c) {
    return c.info.name;
  });
  readSeqTo(names, std::forward<TSeq>(columns));
}

template <typename... Ts>
void BintableColumns::readSeqTo(Column<Ts>&... columns) const {
  readSeqTo(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSeqTo(const std::vector<std::string>& names, TSeq&& columns) const {
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return readIndex(n);
  });
  readSeqTo(indices, std::forward<TSeq>(columns));
}

template <typename... Ts>
void BintableColumns::readSeqTo(const std::vector<std::string>& names, Column<Ts>&... columns) const {
  readSeqTo(names, std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSeqTo(const std::vector<long>& indices, TSeq&& columns) const {
  readSegmentSeqTo(0, indices, columns);
}

template <typename... Ts>
void BintableColumns::readSeqTo(const std::vector<long>& indices, Column<Ts>&... columns) const {
  readSeqTo(indices, std::forward_as_tuple(columns...));
}

// readSegmentSeq

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSegmentSeq(const Segment& rows, const Named<Ts>&... names) const {
  return readSegmentSeq(rows, Indexed<Ts>(readIndex(names))...);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSegmentSeq(const Segment& rows, const Indexed<Ts>&... indices) const {
  std::tuple<VecColumn<Ts>...> columns { { readInfo<Ts>(indices), rows.size() }... };
  readSegmentSeqTo(rows.first, { indices.index... }, columns);
  return columns;
}

// readSegmentSeqTo

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(long firstRow, TSeq&& columns) const {
  const auto names = seqTransform<std::vector<std::string>>(std::forward<TSeq>(columns), [&](const auto& c) {
    return c.info.name;
  });
  readSegmentSeqTo(firstRow, names, std::forward<TSeq>(columns));
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(long firstRow, Column<Ts>&... columns) const {
  readSegmentSeqTo(firstRow, { columns.info.name... }, columns...);
  // Could forward_as_tuple but would be 1 more indirection for the same amount of lines
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(long firstRow, const std::vector<std::string>& names, TSeq&& columns) const {
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return readIndex(n);
  });
  readSegmentSeqTo(firstRow, indices, std::forward<TSeq>(columns));
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(long firstRow, const std::vector<std::string>& names, Column<Ts>&... columns)
    const {
  readSegmentSeqTo(firstRow, names, std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(long firstRow, const std::vector<long>& indices, TSeq&& columns) const {
  const auto bufferSize = readBufferRowCount();
  const long rowCount = columnsRowCount(std::forward<TSeq>(columns));
  const long lastRow = firstRow + rowCount - 1;
  for (Segment src = Segment::fromSize(firstRow, bufferSize), dst = Segment::fromSize(0, bufferSize);
       src.first <= lastRow; // FIXME src += bufferSize, dst += bufferSize) {
       src.first += bufferSize,
               src.last += bufferSize,
               dst.first += bufferSize,
               dst.last += bufferSize) { // FIXME simplify src with only firstRow
    if (dst.last >= rowCount) {
      dst.last = rowCount - 1;
    }
    auto it = indices.begin();
    seqForeach(std::forward<TSeq>(columns), [&](auto& c) {
      auto subcolumn = c.subcolumn(dst);
      readSegmentTo(firstRow, *it, subcolumn);
      ++it;
    });
  }
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(long firstRow, const std::vector<long>& indices, Column<Ts>&... columns) const {
  readSegmentSeqTo(firstRow, indices, std::forward_as_tuple(columns...));
}

// write

template <typename T>
void BintableColumns::write(const Column<T>& column) const {
  writeSegment(0, column);
}

// init

template <typename T>
void BintableColumns::init(const ColumnInfo<T>& info, long index) const {
  auto name = Cfitsio::toCharPtr(info.name);
  auto tform = Cfitsio::toCharPtr(Cfitsio::TypeCode<T>::tform(info.repeatCount));
  int status = 0;
  int cfitsioIndex = index == -1 ? Cfitsio::BintableIo::columnCount(m_fptr) + 1 : index + 1;
  fits_insert_col(m_fptr, cfitsioIndex, name.get(), tform.get(), &status);
  Cfitsio::CfitsioError::mayThrow(status, m_fptr, "Cannot init new column: #" + std::to_string(index));
  if (info.unit != "") {
    const Record<std::string> record { "TUNIT" + std::to_string(cfitsioIndex),
                                       info.unit,
                                       "",
                                       "physical unit of field" };
    Cfitsio::HeaderIo::updateRecord(m_fptr, record);
  }
  // TODO to Cfitsio
}

// writeSegment

template <typename T>
void BintableColumns::writeSegment(long firstRow, const Column<T>& column) const {
  m_edit();
  const auto cfitsioFirstRow = firstRow == -1 ? readRowCount() : firstRow + 1; // 1-based
  Cfitsio::BintableIo::writeColumnSegment(m_fptr, cfitsioFirstRow, column);
}

// writeSeq

template <typename TSeq>
void BintableColumns::writeSeq(TSeq&& columns) const {
  writeSegmentSeq(0, columns);
}

template <typename... Ts>
void BintableColumns::writeSeq(const Column<Ts>&... columns) const {
  writeSeq(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::initSeq(TSeq&& infos, long index) const {
  m_edit();
  auto names = seqTransform<Cfitsio::CStrArray>(infos, [&](const auto& info) {
    return info.name;
  });
  auto tforms = seqTransform<Cfitsio::CStrArray>(infos, [&](const auto& info) {
    return Cfitsio::TypeCode<typename decltype(info)::Value>::tform(info.repeatCount);
  });
  int status = 0;
  int cfitsioIndex = index == -1 ? Cfitsio::BintableIo::columnCount(m_fptr) + 1 : index + 1;
  fits_insert_cols(m_fptr, cfitsioIndex, names.size(), names.data(), tforms.data(), &status);
  // TODO to Cfitsio
  long i = cfitsioIndex;
  seqForeach(infos, [&](const auto& info) {
    if (info.unit != "") {
      const Record<std::string> record { "TUNIT" + std::to_string(i), info.unit, "", "physical unit of field" };
      Cfitsio::HeaderIo::updateRecord(m_fptr, record);
    }
  });
  // TODO to Cfitsio
}

template <typename... Ts>
void BintableColumns::initSeq(const ColumnInfo<Ts>&... infos, long index) const {
  m_edit();
  auto names = Cfitsio::CStrArray({ infos.name... });
  auto tforms = Cfitsio::CStrArray({ Cfitsio::TypeCode<Ts>::tform(infos.repeatCount)... });
  const std::vector<std::string*> tunits { &infos.unit... }; // No vector is needed: this is just easier for looping
  int status = 0;
  int cfitsioIndex = index == -1 ? Cfitsio::BintableIo::columnCount(m_fptr) + 1 : index + 1;
  fits_insert_cols(m_fptr, cfitsioIndex, sizeof...(Ts), names.data(), tforms.data(), &status);
  // TODO to Cfitsio
  long i = cfitsioIndex;
  for (const auto* u : tunits) {
    if (*u != "") {
      const Record<std::string> record { "TUNIT" + std::to_string(i), *u, "", "physical unit of field" };
      Cfitsio::HeaderIo::updateRecord(m_fptr, record);
    }
  }
  // TODO to Cfitsio
  // FIXME initSeq(std::forward_as_tuple(infos...), index); when working
}

// writeSegmentSeq

template <typename TSeq>
void BintableColumns::writeSegmentSeq(long firstRow, TSeq&& columns) const {
  const auto bufferSize = readBufferRowCount();
  const long lastRow = columnsRowCount(std::forward<TSeq>(columns)) - 1;
  for (auto dst = Segment::fromSize(firstRow, bufferSize), src = Segment::fromSize(0, bufferSize);
       dst.first <= lastRow; // FIXME src += bufferSize, dst += bufferSize) {
       src.first += bufferSize, src.last += bufferSize, dst.first += bufferSize, dst.last += bufferSize) {
    if (dst.last > lastRow) {
      dst.last = lastRow;
    }
    seqForeach(std::forward<TSeq>(columns), [&](const auto& c) {
      writeSegment(dst.first, c.subcolumn(src));
    });
  }
}

template <typename... Ts>
void BintableColumns::writeSegmentSeq(long firstRow, const Column<Ts>&... columns) const {
  writeSegmentSeq(firstRow, std::forward_as_tuple(columns...));
}

template <typename TSeq>
long BintableColumns::columnsRowCount(TSeq&& columns) const {
  long rows = -1;
  seqForeach(std::forward<TSeq>(columns), [&](const auto& c) {
    if (rows == -1) {
      rows = c.rowCount();
    } else if (c.rowCount() != rows) {
      throw FitsIOError("Columns do not have the same number of rows."); // FIXME clean
    }
  });
  return rows;
}

} // namespace FitsIO
} // namespace Euclid

#endif
