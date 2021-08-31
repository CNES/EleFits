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

#if defined(_EL_FITSDATA_COLUMN_IMPL) || defined(CHECK_QUALITY)

  #include "EL_FitsData/Column.h"
  #include "EL_FitsData/FitsIOError.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
ColumnInfo<T>::ColumnInfo(const std::string& columnName, const std::string& columnUnit, long columnRepeatCount) :
    name(columnName), unit(columnUnit), repeatCount(columnRepeatCount) {}

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Implementation for Column::rows to dispatch std::string and other types.
 */
template <typename T>
long rowCountDispatchImpl(long elementCount, long repeatCount);

/**
 * std::string dispatch.
 */
template <>
long rowCountDispatchImpl<std::string>(long elementCount, long repeatCount);

/**
 * Other types dispatch.
 */
template <typename T>
long rowCountDispatchImpl(long elementCount, long repeatCount) {
  return (elementCount + repeatCount - 1) / repeatCount;
}

} // namespace Internal
/// @endcond

template <typename T>
Column<T>::Column(ColumnInfo<T> columnInfo) : info(columnInfo) {}

template <typename T>
long Column<T>::rowCount() const {
  return Internal::rowCountDispatchImpl<T>(elementCount(), info.repeatCount);
}

template <typename T>
const T& Column<T>::operator()(long row, long repeat) const {
  const long index = row * info.repeatCount + repeat;
  return *(data() + index);
}

template <typename T>
T& Column<T>::operator()(long row, long repeat) {
  return const_cast<T&>(const_cast<const Column*>(this)->operator()(row, repeat));
}

template <typename T>
const T& Column<T>::at(long row, long repeat) const {
  OutOfBoundsError::mayThrow("Cannot access row index", row, { -rowCount(), rowCount() - 1 });
  OutOfBoundsError::mayThrow("Cannot access repeat index", repeat, { -info.repeatCount, info.repeatCount - 1 });
  const long boundedRow = row < 0 ? rowCount() + row : row;
  const long boundedRepeat = repeat < 0 ? info.repeatCount + repeat : repeat;
  return operator()(boundedRow, boundedRepeat);
}

template <typename T>
T& Column<T>::at(long row, long repeat) {
  return const_cast<T&>(const_cast<const Column*>(this)->at(row, repeat));
}

template <typename T>
T* Column<T>::data() {
  return nullptr;
}

template <typename T>
const PtrColumn<T> Column<T>::subcolumn(const Segment& rows) const {
  return { info, rows.size(), data() + rows.lower };
}

template <typename T>
PtrColumn<T> Column<T>::subcolumn(const Segment& rows) {
  return { info, rows.size(), data() + rows.lower };
}

template <typename T>
PtrColumn<T>::PtrColumn(ColumnInfo<T> columnInfo, long elementCount, const T* data) :
    Column<T>(columnInfo), m_nelements(elementCount), m_cData(data), m_data(nullptr) {}

template <typename T>
PtrColumn<T>::PtrColumn(ColumnInfo<T> columnInfo, long elementCount, T* data) :
    Column<T>(columnInfo), m_nelements(elementCount), m_cData(data), m_data(data) {}

template <typename T>
long PtrColumn<T>::elementCount() const {
  return m_nelements;
}

template <typename T>
const T* PtrColumn<T>::data() const {
  return m_cData;
}

template <typename T>
T* PtrColumn<T>::data() {
  return m_data;
}

template <typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> columnInfo, const std::vector<T>& vecRef) :
    Column<T>(columnInfo), m_cVecPtr(&vecRef), m_vecPtr(nullptr) {}

template <typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> columnInfo, std::vector<T>& vecRef) :
    Column<T>(columnInfo), m_cVecPtr(&vecRef), m_vecPtr(&vecRef) {}

template <typename T>
long VecRefColumn<T>::elementCount() const {
  return m_cVecPtr->size();
}

template <typename T>
const T* VecRefColumn<T>::data() const {
  return m_cVecPtr->data();
}

template <typename T>
T* VecRefColumn<T>::data() {
  return m_vecPtr->data();
}

template <typename T>
const std::vector<T>& VecRefColumn<T>::vector() const {
  return *m_cVecPtr;
}

template <typename T>
VecColumn<T>::VecColumn() : Column<T>({ "", "", 1 }), m_vec() {}

template <typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> columnInfo, std::vector<T> vec) : Column<T>(columnInfo), m_vec(vec) {}

template <typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> columnInfo, long rowCount) :
    Column<T>(columnInfo), m_vec(columnInfo.repeatCount * rowCount) {}

template <>
VecColumn<std::string>::VecColumn(ColumnInfo<std::string> columnInfo, long rowCount);

template <typename T>
long VecColumn<T>::elementCount() const {
  return m_vec.size();
}

template <typename T>
const T* VecColumn<T>::data() const {
  return m_vec.data();
}

template <typename T>
T* VecColumn<T>::data() {
  return const_cast<T*>(const_cast<const VecColumn*>(this)->data());
}

template <typename T>
const std::vector<T>& VecColumn<T>::vector() const {
  return m_vec;
}

template <typename T>
std::vector<T>& VecColumn<T>::vector() {
  return m_vec;
}

  #ifndef DECLARE_COLUMN_CLASSES
    #define DECLARE_COLUMN_CLASSES(type, unused) \
      extern template struct ColumnInfo<type>; \
      extern template class Column<type>; \
      extern template class PtrColumn<type>; \
      extern template class VecRefColumn<type>; \
      extern template class VecColumn<type>;
EL_FITSIO_FOREACH_COLUMN_TYPE(DECLARE_COLUMN_CLASSES)
    #undef DECLARE_COLUMN_CLASSES
  #endif

} // namespace FitsIO
} // namespace Euclid

#endif
