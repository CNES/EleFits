// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_COLUMN_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Column.h"
#include "EleFitsData/FitsError.h"

namespace Euclid {
namespace Fits {

template <typename T, long N, typename TContainer>
constexpr long Column<T, N, TContainer>::Dimension;

template <typename T, long N, typename TContainer>
Column<T, N, TContainer>::Column() : Column(Column<T, N, TContainer>::Info {})
{}

template <typename T, long N, typename TContainer>
const typename Column<T, N, TContainer>::Info& Column<T, N, TContainer>::info() const
{
  return m_info;
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::rename(const std::string& name)
{
  m_info.name = name;
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::reshape(long repeat_count)
{
  // FIXME check that element_count() % repeat_count = 0, but for strings!
  auto shape = Linx::Position<N>::one();
  shape[0] = repeat_count;
  reshape(std::move(shape));
}

template <typename T, long N, typename TContainer>
void Column<T, N, TContainer>::reshape(Linx::Position<N> shape)
{
  // FIXME check that shape is valid
  m_info.shape = std::move(shape);
}

template <typename T, long N, typename TContainer>
long Column<T, N, TContainer>::row_count() const
{
  return this->size() / m_info.element_count();
}

template <typename T, long N, typename TContainer>
const T& Column<T, N, TContainer>::operator()(long row, long repeat) const
{
  const long index = row * m_info.element_count() + repeat;
  return *(this->data() + index);
}

template <typename T, long N, typename TContainer>
T& Column<T, N, TContainer>::operator()(long row, long repeat)
{
  return const_cast<T&>(const_cast<const Column*>(this)->operator()(row, repeat));
}

template <typename T, long N, typename TContainer>
const T& Column<T, N, TContainer>::at(long row, long repeat) const
{
  OutOfBoundsError::may_throw("Cannot access row index", row, {-row_count(), row_count() - 1});
  const auto bound = m_info.element_count();
  OutOfBoundsError::may_throw("Cannot access repeat index", repeat, {-bound, bound - 1});
  const long bounded_row = row < 0 ? row_count() + row : row;
  const long bounded_repeat = repeat < 0 ? bound + repeat : repeat;
  return operator()(bounded_row, bounded_repeat);
}

template <typename T, long N, typename TContainer>
T& Column<T, N, TContainer>::at(long row, long repeat)
{
  return const_cast<T&>(const_cast<const Column*>(this)->at(row, repeat));
}

template <typename T, long N, typename TContainer>
const Linx::PtrRaster<const T, N> Column<T, N, TContainer>::field(long row) const
{
  return Linx::PtrRaster<const T, N>({m_info.shape}, &at(row));
}

template <typename T, long N, typename TContainer>
Linx::PtrRaster<T, N> Column<T, N, TContainer>::field(long row)
{
  return Linx::PtrRaster<T, N>({m_info.shape}, &at(row));
}

template <typename T, long N, typename TContainer>
const PtrColumn<const T, N> Column<T, N, TContainer>::slice(const Segment& rows) const
{
  return PtrColumn<const T, N> {info(), rows.size(), &operator()(rows.front)};
}

template <typename T, long N, typename TContainer>
PtrColumn<T, N> Column<T, N, TContainer>::slice(const Segment& rows)
{
  return PtrColumn<T, N> {info(), rows.size(), &operator()(rows.front)};
}

} // namespace Fits
} // namespace Euclid

#endif
