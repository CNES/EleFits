/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#if defined(_ELEFITSDATA_COLUMN_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Column.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

template <typename T, long N, typename TContainer>
constexpr long Column<T, N, TContainer>::Dim;

template <typename T, long N, typename TContainer>
Column<T, N, TContainer>::Column() : Column(Column<T, N, TContainer>::Info {}) {}

template <typename T, long N, typename TContainer>
Column<T, N, TContainer>::Column(Column<T, N, TContainer>::Info info, long rowCount) :
    Column<T, N, TContainer>::Base(info.elementCount() * rowCount), m_info(std::move(info)) {
  // FIXME assert size() % m_info.elementCount() == 0;
}

template <typename T, long N, typename TContainer>
Column<T, N, TContainer>::Column(Column<T, N, TContainer>::Info info, long rowCount, T* data) :
    Column<T, N, TContainer>::Base(data, data + info.elementCount() * rowCount), m_info(std::move(info)) {
  // FIXME idem
}

template <typename T, long N, typename TContainer>
template <typename... Ts>
Column<T, N, TContainer>::Column(Column<T, N, TContainer>::Info info, Ts&&... args) :
    Column<T, N, TContainer>::Base(std::forward<Ts>(args)...), m_info(std::move(info)) {
  // FIXME idem
}

template <typename T, long N, typename TContainer>
const typename Column<T, N, TContainer>::Info& Column<T, N, TContainer>::info() const {
  return m_info;
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::rename(const std::string& name) {
  m_info.name = name;
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::reshape(long repeatCount) {
  // FIXME check that elementCount() % repeatCount = 0, but for strings!
  auto shape = Position<N>::one();
  shape[0] = repeatCount;
  reshape(std::move(shape));
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::reshape(Position<N> shape) {
  // FIXME check that shape is valid
  m_info.shape = std::move(shape);
}

template <typename T, long N, typename TContainer>
long Column<T, N, TContainer>::elementCount() const {
  return this->size();
}

template <typename T, long N, typename TContainer>
long Column<T, N, TContainer>::rowCount() const {
  return elementCount() / m_info.elementCount();
}

template <typename T, long N, typename TContainer>
const T& Column<T, N, TContainer>::operator()(long row, long repeat) const {
  const long index = row * m_info.elementCount() + repeat;
  return *(this->data() + index);
}

template <typename T, long N, typename TContainer>
T& Column<T, N, TContainer>::operator()(long row, long repeat) {
  return const_cast<T&>(const_cast<const Column*>(this)->operator()(row, repeat));
}

template <typename T, long N, typename TContainer>
const T& Column<T, N, TContainer>::at(long row, long repeat) const {
  OutOfBoundsError::mayThrow("Cannot access row index", row, {-rowCount(), rowCount() - 1});
  const auto bound = m_info.elementCount();
  OutOfBoundsError::mayThrow("Cannot access repeat index", repeat, {-bound, bound - 1});
  const long boundedRow = row < 0 ? rowCount() + row : row;
  const long boundedRepeat = repeat < 0 ? bound + repeat : repeat;
  return operator()(boundedRow, boundedRepeat);
}

template <typename T, long N, typename TContainer>
T& Column<T, N, TContainer>::at(long row, long repeat) {
  return const_cast<T&>(const_cast<const Column*>(this)->at(row, repeat));
}

template <typename T, long N, typename TContainer>
const PtrRaster<const T, N> Column<T, N, TContainer>::entry(long row) const {
  return PtrRaster<const T, N>({m_info.shape}, &at(row));
}

template <typename T, long N, typename TContainer>
PtrRaster<T, N> Column<T, N, TContainer>::entry(long row) {
  return PtrRaster<T, N>({m_info.shape}, &at(row));
}

template <typename T, long N, typename TContainer>
const PtrColumn<const T, N> Column<T, N, TContainer>::slice(const Segment& rows) const {
  return {info(), rows.size(), &operator()(rows.front)};
}

template <typename T, long N, typename TContainer>
PtrColumn<T, N> Column<T, N, TContainer>::slice(const Segment& rows) {
  return {info(), rows.size(), &operator()(rows.front)};
}

#ifndef DECLARE_COLUMN_CLASSES
#define DECLARE_COLUMN_CLASSES(type, unused) \
  extern template struct ColumnInfo<type, 1>; \
  extern template class Column<type, 1, type*>; \
  extern template class Column<const type, 1, const type*>; \
  extern template class Column<type, 1, std::vector<type>>;
ELEFITS_FOREACH_COLUMN_TYPE(DECLARE_COLUMN_CLASSES)
#undef DECLARE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid

#endif
