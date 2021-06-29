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

#if defined(_EL_FITSDATA_RECORDVECTOR_IMPL) || defined(CHECK_QUALITY)

  #include "EL_FitsData/FitsIOError.h"
  #include "EL_FitsData/RecordVector.h"

  #include <algorithm> // find_if

namespace Euclid {
namespace FitsIO {

template <typename T>
RecordVector<T>::RecordVector(std::size_t size) : vector(size) {}

template <typename T>
RecordVector<T>::RecordVector(std::vector<T> records) : vector(std::move(records)) {}

template <typename T>
template <typename... Ts>
RecordVector<T>::RecordVector(const Record<Ts>&... records) : vector { Record<T>(records)... } {}

template <typename T>
const Record<T>& RecordVector<T>::operator[](const std::string& keyword) const {
  const auto it = std::find_if(vector.begin(), vector.end(), [&](const Record<T>& r) {
    return r.keyword == keyword;
  });
  if (it == vector.end()) {
    throw FitsIOError("Cannot find record: " + keyword);
  }
  return *it;
}

template <typename T>
Record<T>& RecordVector<T>::operator[](const std::string& keyword) {
  return const_cast<Record<T>&>(const_cast<const RecordVector<T>*>(this)->operator[](keyword));
}

template <typename T>
template <typename TValue>
Record<TValue> RecordVector<T>::as(const std::string& keyword) const {
  return Record<TValue>(operator[](keyword));
}

} // namespace FitsIO
} // namespace Euclid

#endif
