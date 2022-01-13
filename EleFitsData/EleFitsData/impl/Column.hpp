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

#if defined(_ELEFITSDATA_COLUMN_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Column.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

template <typename T, long N, typename TContainer>
ColumnContainer<T, N, TContainer>::ColumnContainer() : ColumnContainer(ColumnContainer<T, N, TContainer>::Info {}) {}

template <typename T, long N, typename TContainer>
ColumnContainer<T, N, TContainer>::ColumnContainer(ColumnContainer<T, N, TContainer>::Info info, long size) :
    ColumnContainer<T, N, TContainer>::Base(size), m_info(std::move(info)) {
  // FIXME assert size() % m_info.elementCountPerEntry() == 0;
}

template <typename T, long N, typename TContainer>
ColumnContainer<T, N, TContainer>::ColumnContainer(ColumnContainer<T, N, TContainer>::Info info, long size, T* data) :
    ColumnContainer<T, N, TContainer>::Base(data, data + size), m_info(std::move(info)) {
  // FIXME idem
}

template <typename T, long N, typename TContainer>
template <typename... Ts>
ColumnContainer<T, N, TContainer>::ColumnContainer(ColumnContainer<T, N, TContainer>::Info info, Ts&&... args) :
    ColumnContainer<T, N, TContainer>::Base(std::forward<Ts>(args)...), m_info(std::move(info)) {
  // FIXME idem
}

template <typename T, long N, typename TContainer>
const typename ColumnContainer<T, N, TContainer>::Info& ColumnContainer<T, N, TContainer>::info() const {
  return m_info;
}

template <typename T, long N, typename TContainer>
void ColumnContainer<T, N, TContainer>::rename(const std::string& name) {
  m_info.name = name;
}

template <typename T, long N, typename TContainer>
void ColumnContainer<T, N, TContainer>::reshape(long repeatCount) {
  // FIXME check that elementCount() % repeatCount() = 0, but for strings!
  m_info.shape[0] = repeatCount;
}

template <typename T, long N, typename TContainer>
long ColumnContainer<T, N, TContainer>::elementCount() const {
  return this->size();
}

template <typename T, long N, typename TContainer>
long ColumnContainer<T, N, TContainer>::rowCount() const {
  return elementCount() / m_info.elementCountPerEntry();
}

template <typename T, long N, typename TContainer>
const T& ColumnContainer<T, N, TContainer>::operator()(long row, long repeat) const {
  const long index = row * m_info.elementCountPerEntry() + repeat;
  return *(this->data() + index);
}

template <typename T, long N, typename TContainer>
T& ColumnContainer<T, N, TContainer>::operator()(long row, long repeat) {
  return const_cast<T&>(const_cast<const ColumnContainer*>(this)->operator()(row, repeat));
}

template <typename T, long N, typename TContainer>
const T& ColumnContainer<T, N, TContainer>::at(long row, long repeat) const {
  OutOfBoundsError::mayThrow("Cannot access row index", row, {-rowCount(), rowCount() - 1});
  const auto bound = m_info.elementCountPerEntry();
  OutOfBoundsError::mayThrow("Cannot access repeat index", repeat, {-bound, bound - 1});
  const long boundedRow = row < 0 ? rowCount() + row : row;
  const long boundedRepeat = repeat < 0 ? bound + repeat : repeat;
  return operator()(boundedRow, boundedRepeat);
}

template <typename T, long N, typename TContainer>
T& ColumnContainer<T, N, TContainer>::at(long row, long repeat) {
  return const_cast<T&>(const_cast<const ColumnContainer*>(this)->at(row, repeat));
}

template <typename T, long N, typename TContainer>
const PtrColumn<const T> ColumnContainer<T, N, TContainer>::slice(const Segment& rows) const {
  return {info(), rows.size(), &operator()(rows.front)};
}

template <typename T, long N, typename TContainer>
PtrColumn<T> ColumnContainer<T, N, TContainer>::slice(const Segment& rows) {
  return {info(), rows.size(), &operator()(rows.front)};
}

#ifndef DECLARE_COLUMN_CLASSES
#define DECLARE_COLUMN_CLASSES(type, unused) \
  extern template struct ColumnInfo<type, 1>; \
  extern template class ColumnContainer<type, 1, type*>; \
  extern template class ColumnContainer<const type, 1, const type*>; \
  extern template class ColumnContainer<type, 1, std::vector<type>>;
ELEFITS_FOREACH_COLUMN_TYPE(DECLARE_COLUMN_CLASSES)
#undef DECLARE_COLUMN_CLASSES
#endif

} // namespace Fits
} // namespace Euclid

#endif
