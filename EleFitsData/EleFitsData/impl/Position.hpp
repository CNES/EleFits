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

#if defined(_ELEFITSDATA_POSITION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Position.h"

namespace Euclid {
namespace Fits {

template <long N>
Position<N>::Position() : DataContainer<long, Indices<N>, Position<N>>() {}

template <long N>
Position<N>::Position(long dim) : DataContainer<long, Indices<N>, Position<N>>(dim) {}

template <long N>
Position<N>::Position(std::initializer_list<long> indices) : DataContainer<long, Indices<N>, Position<N>>(indices) {}

template <long N>
template <typename TIterator>
Position<N>::Position(TIterator begin, TIterator end) : DataContainer<long, Indices<N>, Position<N>>(begin, end) {}

template <long N>
Position<N> Position<N>::zero() {
  Position<N> res(std::abs(Dim));
  std::fill(res.begin(), res.end(), 0);
  return res;
}

template <long N>
Position<N> Position<N>::max() {
  Position<N> res(std::abs(N));
  std::fill(res.begin(), res.end(), -1);
  return res;
}

template <long N>
bool Position<N>::isZero() const {
  for (auto i : *this) {
    if (i != 0) {
      return false;
    }
  }
  return true;
}

template <long N>
bool Position<N>::isMax() const {
  for (auto i : *this) {
    if (i != -1) {
      return false;
    }
  }
  return true;
}

template <long N>
template <long M>
Position<M> Position<N>::slice() const {
  const auto b = this->begin();
  auto e = b;
  std::advance(e, M);
  return Position<M>(b, e);
}

template <long N>
template <long M>
Position<M> Position<N>::extend(const Position<M>& padding) const {
  auto res = padding;
  for (std::size_t i = 0; i < this->size(); ++i) { // TODO std::transform
    res[i] = (*this)[i];
  }
  return res;
}

} // namespace Fits
} // namespace Euclid

#endif
