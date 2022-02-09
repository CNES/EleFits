// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_RECORDVECTOR_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/FitsError.h"
#include "EleFitsData/RecordVec.h"

#include <algorithm> // find_if

namespace Euclid {
namespace Fits {

template <typename T>
RecordVec<T>::RecordVec(std::size_t size) : vector(size) {}

template <typename T>
RecordVec<T>::RecordVec(const std::vector<Record<T>>& records) : vector(records) {}

template <typename T>
RecordVec<T>::RecordVec(std::vector<Record<T>>&& records) : vector(std::move(records)) {}

template <typename T>
RecordVec<T>::RecordVec(std::initializer_list<Record<T>> records) : vector(std::move(records)) {}

template <typename T>
template <typename... Ts>
RecordVec<T>::RecordVec(const Record<Ts>&... records) : vector {Record<T>(records)...} {}

template <typename T>
const Record<T>& RecordVec<T>::operator[](const std::string& keyword) const {
  const auto it = std::find_if(vector.begin(), vector.end(), [&](const Record<T>& r) {
    return r.keyword == keyword;
  });
  if (it == vector.end()) {
    throw FitsError("Cannot find record: " + keyword);
  }
  return *it;
}

template <typename T>
Record<T>& RecordVec<T>::operator[](const std::string& keyword) {
  return const_cast<Record<T>&>(const_cast<const RecordVec<T>*>(this)->operator[](keyword));
}

template <typename T>
template <typename TValue>
Record<TValue> RecordVec<T>::as(const std::string& keyword) const {
  return Record<TValue>(operator[](keyword));
}

} // namespace Fits
} // namespace Euclid

#endif
