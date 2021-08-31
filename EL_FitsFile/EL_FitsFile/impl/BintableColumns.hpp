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
  return Cfitsio::Bintable::readColumnInfo<T>(m_fptr, index + 1);
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
  Cfitsio::Bintable::readColumnSegment<T>(m_fptr, cfitsioRows, index + 1, column);
}

// readSeq

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Named<Ts>&... names) const {
  return readSeq(Indexed<Ts>(readIndex(names.name))...);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Indexed<Ts>&... indices) const {
  m_touch();
  const std::vector<long> cfitsioIndices { (indices.index + 1)... }; // 1-based
  return Cfitsio::Bintable::readColumns<Ts...>(m_fptr, cfitsioIndices); // FIXME through readSegmentSeqTo
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
  std::tuple<VecColumn<Ts>...> columns { { readInfo<Ts>(index), rows.size() }... };
  readSegmentSeqTo(rows, columns);
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
  readSegmentSeqTo(rows, { columns.info.name... }, columns...);
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
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return readIndex(n);
  });
  readSegmentSeqTo(rows, indices, columns...);
}

template <typename TSeq>
void BintableColumns::readSegmentSeqTo(const Segment& rows, const std::vector<long>& indices, TSeq&& columns) const {
  long bufferSize = 0;
  int status = 0;
  fits_get_rowsize(m_fptr, &bufferSize, &status);
  // FIXME mayThrow
  // FIXME const auto bufferSize = Cfitsio::Bintable::bufferSize(m_fptr);
  for (Segment src = Segment::fromSize(rows.lower, bufferSize), dst = Segment::fromSize(0, bufferSize);
       src.lower <= rows.upper; // FIXME src += bufferSize, dst += bufferSize) {
       src.lower += bufferSize, src.upper += bufferSize, dst.lower += bufferSize, dst.upper += bufferSize) {
    if (src.upper > rows.upper) {
      src.upper = rows.upper;
    }
    auto it = indices.begin();
    seqForeach(columns, [&](auto& c) {
      readSegmentTo(src, *it, c.subcolumn(dst));
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
  m_edit();
  Cfitsio::Bintable::writeColumn(m_fptr, column);
}

// insert

template <typename T>
void BintableColumns::insert(const Column<T>& column, long index) const {
  m_edit();
  if (index == -1) { // TODO handle other negative values?
    Cfitsio::Bintable::appendColumn(m_fptr, column);
  } else {
    Cfitsio::Bintable::insertColumn(m_fptr, index + 1, column);
  }
}

// FIXME writeSegment

// FIXME insertSegment

// writeSeq

template <typename... Ts>
void BintableColumns::writeSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::writeColumns(m_fptr, columns...); // FIXME test
}

template <typename TSeq>
void BintableColumns::writeSeq(TSeq&& columns) const {
  // FIXME implement
}

// appendSeq

template <typename TSeq>
void BintableColumns::appendSeq(TSeq&& columns) const {
  // FIXME implement
}

template <typename... Ts>
void BintableColumns::appendSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::appendColumns(m_fptr, columns...);
}

// FIXME writeSegmentSeq

// FIXME appendSegmentSeq

} // namespace FitsIO
} // namespace Euclid

#endif
