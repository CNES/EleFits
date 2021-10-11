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

#ifndef _EL_FITSDATA_RECORDVECTOR_H
#define _EL_FITSDATA_RECORDVECTOR_H

#include "EL_FitsData/DataUtils.h"
#include "EL_FitsData/Record.h"

#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup header_data_classes
 * @brief A vector of records with find and conversion services.
 * @tparam T The value type of the records
 * @details
 * Alias `RecordSeq` is provided for `T` = `VariantValue`.
 */
template <typename T>
class RecordVector { // FIXME rename as RecordVec in 4.0

public:
  /**
   * @brief Destructor.
   */
  ~RecordVector() = default;

  /**
   * @brief Create a RecordVector with given number of records.
   */
  explicit RecordVector(std::size_t size);

  /**
   * @brief Create a RecordVector from a vector of records.
   */
  RecordVector(const std::vector<Record<T>>& records);

  /**
   * @brief Create a RecordVector from a vector of records.
   */
  RecordVector(std::vector<Record<T>>&& records);

  /**
   * @brief Create a RecordVector from a list of records.
   */
  template <typename... Ts>
  RecordVector(const Record<Ts>&... records);

  /**
   * @brief The records.
   */
  std::vector<Record<T>> vector;

  /**
   * @brief Get an iterator to the beginning.
   */
  typename std::vector<Record<T>>::const_iterator begin() const {
    return vector.begin();
  }

  /**
   * @brief Get an iterator to the end.
   */
  typename std::vector<Record<T>>::const_iterator end() const {
    return vector.end();
  }

  /**
   * @copydoc begin()
   */
  typename std::vector<Record<T>>::iterator begin() {
    return vector.begin();
  }

  /**
   * @copydoc end()
   */
  typename std::vector<Record<T>>::iterator end() {
    return vector.end();
  }

  /**
   * @brief Find the first record with given keyword.
   */
  const Record<T>& operator[](const std::string& keyword) const;

  /**
   * @brief Find the first record with given keyword.
   */
  Record<T>& operator[](const std::string& keyword);

  /**
   * @brief Find and cast the first record with given keyword.
   * @tparam TValue The destination value type.
   * @details
   * Although the method returns a Record, it can itself be sliced as its value,
   * so the following works:
   * \code
   * RecordSeq records; // Equivalent to RecordVector<VariantValue>
   * // ...
   * int i = records.as<int>("KEYWORD"); // Get the value as int
   * \endcode
   */
  template <typename TValue>
  Record<TValue> as(const std::string& keyword) const;
};

/**
 * @brief A sequence of records of any type.
 */
using RecordSeq = RecordVector<VariantValue>;

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSDATA_RECORDVECTOR_IMPL
#include "EL_FitsData/impl/RecordVector.hpp"
#undef _EL_FITSDATA_RECORDVECTOR_IMPL
/// @endcond

#endif
