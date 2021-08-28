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

template <typename T>
VecColumn<T> BintableColumns::read(const std::string& name) const {
  return read<T>(Cfitsio::Bintable::columnIndex(m_fptr, name) - 1);
}

template <typename T>
VecColumn<T> BintableColumns::read(long index) const {
  const auto rows = Cfitsio::Bintable::rowCount(m_fptr);
  return readSegment<T>({ 0, rows - 1 }, index);
}

template <typename T>
void BintableColumns::readTo(Column<T>& column) const {
  readTo<T>(column.info.name, column);
}

template <typename T>
void BintableColumns::readTo(const std::string& name, Column<T>& column) const {
  readTo<T>(Cfitsio::Bintable::columnIndex(m_fptr, name) - 1, column);
}

template <typename T>
void BintableColumns::readTo(long index, Column<T>& column) const {
  const auto rows = Cfitsio::Bintable::rowCount(m_fptr);
  readSegmentTo<T>({ 0, rows - 1 }, column);
}

template <typename T>
VecColumn<T> BintableColumns::readSegment(const Segment& rows, const std::string& name) const {
  return readSegment<T>(rows, Cfitsio::Bintable::columnIndex(m_fptr, name) - 1);
}

template <typename T>
VecColumn<T> BintableColumns::readSegment(const Segment& rows, long index) const {
  VecColumn<T> column(Cfitsio::Bintable::readColumnInfo<T>(m_fptr, index + 1), rows.size());
  readSegmentTo<T>(rows, index, column);
  return column;
}

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, Column<T>& column) const {
  readSegmentTo<T>(rows, column.info.name, column);
}

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, const std::string& name, Column<T>& column) const {
  readSegmentTo<T>(rows, Cfitsio::Bintable::columnIndex(m_fptr, name), column);
}

template <typename T>
void BintableColumns::readSegmentTo(const Segment& rows, long index, Column<T>& column) const {
  m_touch();
  const Segment cfitsioRows { rows.lower + 1, rows.upper + 1 };
  Cfitsio::Bintable::readColumnSegment<T>(m_fptr, cfitsioRows, index + 1, column);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Named<Ts>&... names) const {
  m_touch();
  const std::vector<std::string> nameVector { names.name... };
  return Cfitsio::Bintable::readColumns<Ts...>(m_fptr, nameVector);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableColumns::readSeq(const Indexed<Ts>&... indices) const {
  m_touch();
  const std::vector<long> cfitsioIndices { (indices.index + 1)... }; // 1-based
  return Cfitsio::Bintable::readColumns<Ts...>(m_fptr, cfitsioIndices);
}

template <typename T>
void BintableColumns::write(const Column<T>& column) const {
  m_edit();
  Cfitsio::Bintable::writeColumn(m_fptr, column);
}

template <typename T>
void BintableColumns::insert(const Column<T>& column, long index) const {
  m_edit();
  if (index == -1) { // TODO handle other negative values?
    Cfitsio::Bintable::appendColumn(m_fptr, column);
  } else {
    Cfitsio::Bintable::insertColumn(m_fptr, index + 1, column);
  }
}

template <typename... Ts>
void BintableColumns::writeSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::writeColumns(m_fptr, columns...); // FIXME test
}

template <typename TSeq>
void BintableColumns::writeSeq(TSeq&& columns) const {
  tupleApply(columns, this->writeSeq<TSeq>); // FIXME test
}

template <typename... Ts>
void BintableColumns::appendSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::appendColumns(m_fptr, columns...);
}

} // namespace FitsIO
} // namespace Euclid

#endif
