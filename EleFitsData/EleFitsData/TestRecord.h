// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_TESTRECORD_H
#define _ELEFITSDATA_TESTRECORD_H

#include "EleFitsData/Record.h"
#include "EleFitsData/RecordVec.h"
#include "EleFitsData/TestUtils.h"

#include <algorithm>
#include <complex>
#include <string>

namespace Fits {
namespace Test {

/**
 * @brief Generate a random record.
 * @tparam T The value type.
 * @param typename The type name as a string, e.g. `"unsigned long int"`.
 * @details
 * In `"unsigned long int"`, `"unsigned"` and `"long"` are prefixes, and `"int"` is the suffix.
 * The generated record is as follows:
 * - The keyword is made of the first letter of the prefixes and the suffix, capitalized, e.g. `"ULINT"`;
 * - The value is random;
 * - The unit is made of the first letter of the prefixes, e.g. `"ul"`;
 * - The comment is made of the first letter of the prefixes, a space, and the suffix, e.g. `"ul int"`.
 */
template <typename T>
static Record<T> generate_random_record(const std::string& type_name);

/**
 * @brief Generate a random record.
 */
template <typename T>
static Record<T> generate_random_record(const std::string& k, const std::string& u, const std::string& c);

/**
 * @brief A random set of Records.
 * @details
 * All compatible types of Records are generated, but `VariantValue`.
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
  const Record<T>& get_record() const;

  /**
   * @brief Get the record with given type.
   */
  template <typename T>
  Record<T>& get_record();

  /**
   * @brief Get all the records.
   */
  RecordSeq all_record() const;

  /** @brief The number of records. */
  static constexpr long record_count = 16;
};

} // namespace Test
} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_TESTRECORD_IMPL
#include "EleFitsData/impl/TestRecord.hpp"
#undef _ELEFITSDATA_TESTRECORD_IMPL
/// @endcond

#endif
