// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_RECORDVECTOR_H
#define _ELEFITSDATA_RECORDVECTOR_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Record.h"

#include <vector>

namespace Fits {

/**
 * @ingroup header_data_classes
 * @brief A vector of records with find and conversion services.
 * @tparam T The value type of the records
 * @details
 * Alias `RecordSeq` is provided for `T` = `VariantValue`.
 */
template <typename T>
class RecordVec {
public:

  /**
   * @brief Destructor.
   */
  ~RecordVec() = default;

  /**
   * @brief Create a RecordVec with given number of records.
   */
  explicit RecordVec(std::size_t size);

  /**
   * @brief Create a RecordVec from a vector of records.
   */
  RecordVec(const std::vector<Record<T>>& records);

  /**
   * @brief Create a RecordVec from a vector of records.
   */
  RecordVec(std::vector<Record<T>>&& records);

  /**
   * @brief Create a `RecordVec` from a list of homogeneous records.
   */
  RecordVec(std::initializer_list<Record<T>> records);

  /**
   * @brief Create a RecordVec from a list of heterogeneous records.
   */
  template <typename... Ts>
  RecordVec(const Record<Ts>&... records);

  /**
   * @brief The records.
   */
  std::vector<Record<T>> vector;

  /**
   * @brief Get an iterator to the beginning.
   */
  typename std::vector<Record<T>>::const_iterator begin() const
  {
    return vector.begin();
  }

  /**
   * @copydoc begin()const
   */
  typename std::vector<Record<T>>::iterator begin()
  {
    return vector.begin();
  }

  /**
   * @brief Get an iterator to the end.
   */
  typename std::vector<Record<T>>::const_iterator end() const
  {
    return vector.end();
  }

  /**
   * @copydoc end()const
   */
  typename std::vector<Record<T>>::iterator end()
  {
    return vector.end();
  }

  /**
   * @brief Check whether a given keyword is present.
   */
  bool has(const std::string& keyword) const;

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
   * RecordSeq records; // Equivalent to RecordVec<VariantValue>
   * // ...
   * int i = records.as<int>("KEYWORD"); // Get the value as int
   * \endcode
   */
  template <typename TValue>
  Record<TValue> as(const std::string& keyword) const;
};

/**
 * @ingroup header_data_classes
 * @brief A sequence of records of any type.
 */
using RecordSeq = RecordVec<VariantValue>;

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_RECORDVECTOR_IMPL
#include "EleFitsData/impl/RecordVec.hpp"
#undef _ELEFITSDATA_RECORDVECTOR_IMPL
/// @endcond

#endif
