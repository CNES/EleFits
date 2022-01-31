// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_COLUMNKEY_H
#define _ELEFITS_COLUMNKEY_H

#include <boost/optional.hpp>
#include <string>

namespace Euclid {
namespace Fits {

// Forward declaration for lazy evaluation
class BintableColumns;

/**
 * @brief A lazy-evaluated column key initialized either with an index or a name.
 * @details
 * If missing at the time of usage, the index or name is evaluated thanks to a `BintableColumns` object.
 * Negative (backward) indices are also resolved lazily.
 * After evaluation, the value is cached.
 * @note
 * Constructors are intentionally not explicit.
 */
class ColumnKey {
public:
  /**
   * @brief Create a column key from an index.
   */
  ColumnKey(long index);

  /**
   * @brief Create a column key from an index.
   */
  ColumnKey(int index);

  /**
   * @brief Create a column key from a name.
   */
  ColumnKey(const std::string& name);

  /**
   * @brief Create a column key from a name.
   */
  ColumnKey(const char* name);

  /**
   * @brief Get the evaluated index.
   */
  long index(const BintableColumns& columns);

  /**
   * @brief Get the evaluated name.
   */
  const std::string& name(const BintableColumns& columns);

private:
  /**
   * @brief The possibly missing index.
   */
  boost::optional<long> m_index;

  /**
   * @brief The possibly missing name.
   */
  boost::optional<std::string> m_name;
};

} // namespace Fits
} // namespace Euclid

#endif
