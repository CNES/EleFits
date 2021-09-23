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

// readInfo

template <typename T>
ColumnInfo<T> BintableColumns::readInfo(const std::string& name) const {
  return readInfo<T>(readIndex(name));
}

template <typename T>
ColumnInfo<T> BintableColumns::readInfo(long index) const {
  return Cfitsio::BintableIo::readColumnInfo<T>(m_fptr, index + 1);
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
  readSegmentTo<T>(rows, index, column);
  return column;
}

// readSegmentTo

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, Column<T>& column) const {
  readSegmentTo<T>(rows, column.info.name, column);
}

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, const std::string& name, Column<T>& column) const {
  readSegmentTo<T>(rows, readIndex(name), column);
}

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, long index, Column<T>& column) const {
  m_touch();
  const Segment cfitsioRows { rows.lower + 1, rows.upper + 1 }; // FIXME rows + 1
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
  const auto names = seqTransform<std::vector<std::string>>(columns, [&](auto c) { // FIXME forward
    return c.info.name;
  });
  readSeqTo(names, columns); // FIXME forward
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
  readSeqTo(indices, columns); // FIXME forward
}

template <typename... Ts>
void BintableColumns::readSeqTo(const std::vector<std::string>& names, Column<Ts>&... columns) const {
  readSeqTo(names, std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSeqTo(const std::vector<long>& indices, TSeq&& columns) const {
  readSegmentSeqTo({ 0, readRowCount() - 1 }, indices, columns);
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
  readSegmentSeqTo(rows, { indices.index... }, columns);
  return columns;
}

// readSegmentSeqTo

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(const Segment& rows, TSeq&& columns) const {
  const auto names = seqTransform<std::vector<std::string>>(columns, [&](auto c) { // FIXME forward
    return c.info.name;
  });
  readSegmentSeqTo(rows, names, columns); // FIXME forward
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(const Segment& rows, Column<Ts>&... columns) const {
  readSegmentSeqTo(rows, { columns.info.name... }, columns...); // TODO forward_as_tuple?
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(const Segment& rows, const std::vector<std::string>& names, TSeq&& columns)
    const {
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return readIndex(n);
  });
  readSegmentSeqTo(rows, indices, columns); // FIXME forward
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(
    const Segment& rows,
    const std::vector<std::string>& names,
    Column<Ts>&... columns) const {
  readSegmentSeqTo(rows, names, std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(const Segment& rows, const std::vector<long>& indices, TSeq&& columns) const {
  const auto bufferSize = readBufferRowCount();
  for (Segment src = Segment::fromSize(rows.lower, bufferSize), dst = Segment::fromSize(0, bufferSize);
       src.lower <= rows.upper; // FIXME src += bufferSize, dst += bufferSize) {
       src.lower += bufferSize, src.upper += bufferSize, dst.lower += bufferSize, dst.upper += bufferSize) {
    if (src.upper > rows.upper) {
      src.upper = rows.upper;
    }
    auto it = indices.begin();
    seqForeach(columns, [&](auto& c) {
      auto subcolumn = c.subcolumn(dst);
      readSegmentTo(src, *it, subcolumn);
      ++it;
    });
  }
}

template <typename... Ts>
void BintableColumns::readSegmentSeqTo(const Segment& rows, const std::vector<long>& indices, Column<Ts>&... columns)
    const {
  readSegmentSeqTo(rows, indices, std::forward_as_tuple(columns...));
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
    Cfitsio::HeaderIo::writeRecord(m_fptr, record);
  }
  // FIXME to Cfitsio
}

// writeSegment

template <typename T>
void BintableColumns::writeSegment(long firstRow, const Column<T>& column) const {
  m_edit();
  Cfitsio::BintableIo::writeColumnSegment(m_fptr, firstRow, column);
}

// writeSeq

template <typename TSeq>
void BintableColumns::writeSeq(TSeq&& columns) const {
  long rowCount = 0;
  seqForeach(columns, [&](const auto& c) {
    rowCount = std::max(rowCount, c.rowCount()); // Needed if rows becomes firstRow
  });
  writeSegmentSeq({ 0, rowCount - 1 }, columns);
}

template <typename... Ts>
void BintableColumns::writeSeq(const Column<Ts>&... columns) const {
  writeSeq(std::forward_as_tuple(columns...));
}

template <typename TSeq>
void BintableColumns::initSeq(TSeq&& infos, long index) const {
  // FIXME implement
}

template <typename... Ts>
void BintableColumns::initSeq(const ColumnInfo<Ts>&... infos, long index) const {
  // FIXME implement
  auto names = Cfitsio::CStrArray({ infos.name... });
  auto tforms = Cfitsio::CStrArray({ Cfitsio::TypeCode<Ts>::tform(infos.repeatCount)... });
  const std::vector<std::string*> tunits {
    &infos.unit...
  }; // TODO no vector is needed: this is just easier for looping
  int status = 0;
  int cfitsioIndex = index == -1 ? Cfitsio::BintableIo::columnCount(m_fptr) + 1 : index + 1;
  fits_insert_cols(m_fptr, cfitsioIndex, sizeof...(Ts), names.data(), tforms.data(), &status);
  // FIXME to Cfitsio
  long i = cfitsioIndex;
  for (const auto* u : tunits) {
    if (*u != "") {
      const Record<std::string> record { "TUNIT" + std::to_string(i), *u, "", "physical unit of field" };
      Cfitsio::HeaderIo::writeRecord(m_fptr, record);
    }
  }
}

// FIXME removeSeq

template <typename TSeq>
void BintableColumns::writeSegmentSeq(const Segment& rows, TSeq&& columns) const {
  const auto bufferSize = readBufferRowCount();
  for (auto dst = Segment::fromSize(rows.lower, bufferSize), src = Segment::fromSize(0, bufferSize);
       dst.lower <= rows.upper; // FIXME src += bufferSize, dst += bufferSize) {
       src.lower += bufferSize, src.upper += bufferSize, dst.lower += bufferSize, dst.upper += bufferSize) {
    if (dst.upper > rows.upper) {
      dst.upper = rows.upper;
    }
    seqForeach(columns, [&](const auto& c) {
      writeSegment(dst.lower, c.subcolumn(src));
    });
  }
}

template <typename... Ts>
void BintableColumns::writeSegmentSeq(const Segment& rows, const Column<Ts>&... columns) const {
  writeSegmentSeq(rows, std::forward_as_tuple(columns...));
}

} // namespace FitsIO
} // namespace Euclid

#endif
