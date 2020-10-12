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

#ifndef _EL_FITSDATA_TESTRECORD_H
#define _EL_FITSDATA_TESTRECORD_H

#include <algorithm>
#include <complex>
#include <string>

#include "EL_FitsData/TestUtils.h"
#include "EL_FitsData/Record.h"
#include "EL_FitsData/RecordVector.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief Generate a random record.
 * @tparam T The value type.
 * @param typeName The type name as a string, e.g. `"unsigned long int"`.
 * @details
 * In `"unsigned long int"`, `"unsigned"` and `"long"` are prefixes, and `"int"` is the suffix.
 * The generated record is as follows:
 * - The keyword is made of the first letter of the prefixes and the suffix, capitalized, e.g. `"ULINT"`;
 * - The value is random;
 * - The unit is made of the first letter of the prefixes, e.g. `"ul"`;
 * - The comment is made of the first letter of the prefixes, a space, and the suffix, e.g. `"ul int"`.
 */
template <typename T>
static Record<T> generateRandomRecord(const std::string &typeName);

/**
 * @brief Generate a random record.
 */
template <typename T>
static Record<T> generateRandomRecord(const std::string &k, const std::string &u, const std::string &c);

/**
 * @brief A random set of Records.
 * @details
 * All compatible types of Records are generated, but `boost::any`.
 */
struct RandomHeader {

  /**
   * @brief Constructor.
   */
  RandomHeader();

  /** @brief bool */
  Record<bool> b;
  /** @brief char */
  Record<char> c;
  /** @brief short */
  Record<short> s;
  /** @brief int */
  Record<int> i;
  /** @brief long */
  Record<long> l;
  /** @brief long long */
  Record<long long> ll;
  /** @brief unsigned char */
  Record<unsigned char> uc;
  /** @brief unsigned short */
  Record<unsigned short> us;
  /** @brief unsigned int */
  Record<unsigned int> ui;
  /** @brief unsigned long */
  Record<unsigned long> ul;
  /** @brief unsigned long long */
  Record<unsigned long long> ull;
  /** @brief float */
  Record<float> f;
  /** @brief double */
  Record<double> d;
  /** @brief std::complex<float> */
  Record<std::complex<float>> cf;
  /** @brief std::complex<double> */
  Record<std::complex<double>> cd;
  /** @brief std::string */
  Record<std::string> str;

  /**
   * @brief Get the record with given type.
   */
  template <typename T>
  const Record<T> &getRecord() const;

  /**
   * @brief Get the record with given type.
   */
  template <typename T>
  Record<T> &getRecord();

  /**
   * @brief Get all the records.
   */
  RecordVector<boost::any> allRecords() const;

  /** @brief The number of records. */
  static constexpr long recordCount = 16;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSDATA_TESTRECORD_IMPL
#include "EL_FitsData/impl/TestRecord.hpp"
#undef _EL_FITSDATA_TESTRECORD_IMPL
/// @endcond

#endif
