// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_POSITION_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/Position.h"

namespace Euclid {
namespace Fits {

template <long N>
Position<N>::Position() : DataContainer<long, DataContainerHolder<long, Indices<N>>, Position<N>>() {}

template <long N>
Position<N>::Position(long dim) : DataContainer<long, DataContainerHolder<long, Indices<N>>, Position<N>>(dim) {}

template <long N>
Position<N>::Position(std::initializer_list<long> indices) :
    DataContainer<long, DataContainerHolder<long, Indices<N>>, Position<N>>(indices) {}

template <long N>
template <typename TIterator>
Position<N>::Position(TIterator begin, TIterator end) :
    DataContainer<long, DataContainerHolder<long, Indices<N>>, Position<N>>(begin, end) {}

template <long N>
Position<N> Position<N>::zero() {
  Position<N> res(std::abs(Dim));
  std::fill(res.begin(), res.end(), 0);
  return res;
}

template <long N>
Position<N> Position<N>::one() {
  Position<N> res(std::abs(Dim));
  std::fill(res.begin(), res.end(), 1);
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
