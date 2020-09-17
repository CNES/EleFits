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

#include "EL_FitsData/Column.h"

namespace Euclid {
namespace FitsIO {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Implementation for Column::rows to dispatch std::string and other types.
 */
template <typename T>
long rowsImpl(long nelements, long repeat);

/**
 * std::string dispatch.
 */
template <>
long rowsImpl<std::string>(long nelements, long repeat);

/**
 * Other types dispatch.
 */
template <typename T>
long rowsImpl(long nelements, long repeat) {
  return (nelements + repeat - 1) / repeat;
}

} // namespace Internal
/// @endcond

template <typename T>
Column<T>::Column(ColumnInfo<T> columnInfo) : info(columnInfo) {
}

template <typename T>
long Column<T>::rows() const {
  return Internal::rowsImpl<T>(nelements(), info.repeat);
}

template <typename T>
PtrColumn<T>::PtrColumn(ColumnInfo<T> columnInfo, long nelements, const T *data) :
    Column<T>(columnInfo),
    m_nelements(nelements),
    m_data(data) {
}

template <typename T>
long PtrColumn<T>::nelements() const {
  return m_nelements;
}

template <typename T>
const T *PtrColumn<T>::data() const {
  return m_data;
}

template <typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> columnInfo, const std::vector<T> &vectorRef) :
    Column<T>(columnInfo),
    m_ref(vectorRef) {
}

template <typename T>
long VecRefColumn<T>::nelements() const {
  return m_ref.size();
}

template <typename T>
const T *VecRefColumn<T>::data() const {
  return m_ref.data();
}

template <typename T>
const std::vector<T> &VecRefColumn<T>::vector() const {
  return m_ref;
}

template <typename T>
VecColumn<T>::VecColumn() : Column<T>({ "", "", 1 }), m_vec() {
}

template <typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> columnInfo, std::vector<T> vector) : Column<T>(columnInfo), m_vec(vector) {
}

template <typename T>
long VecColumn<T>::nelements() const {
  return m_vec.size();
}

template <typename T>
const T *VecColumn<T>::data() const {
  return m_vec.data();
}

template <typename T>
T *VecColumn<T>::data() {
  return m_vec.data();
}

template <typename T>
const std::vector<T> &VecColumn<T>::vector() const {
  return m_vec;
}

template <typename T>
std::vector<T> &VecColumn<T>::vector() {
  return m_vec;
}

} // namespace FitsIO
} // namespace Euclid
