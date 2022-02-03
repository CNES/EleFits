// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_RECORD_H
#define _ELEFITSDATA_RECORD_H

#include <boost/any.hpp>
#include <complex>
#include <string>

namespace Euclid {
namespace Fits {

/**
 * @ingroup header_data_classes
 * @brief Loop over supported record types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EleFitsPrintSupportedTypes to display all supported types
 * @see ELEFITS_FOREACH_RASTER_TYPE
 * @see ELEFITS_FOREACH_COLUMN_TYPE
 */
#define ELEFITS_FOREACH_RECORD_TYPE(MACRO) \
  MACRO(bool, bool) \
  MACRO(char, char) \
  MACRO(short, short) \
  MACRO(int, int) \
  MACRO(long, long) \
  MACRO(long long, longlong) \
  MACRO(float, float) \
  MACRO(double, double) \
  MACRO(std::complex<float>, complex_float) \
  MACRO(std::complex<double>, complex_double) \
  MACRO(std::string, string) \
  MACRO(unsigned char, uchar) \
  MACRO(unsigned short, ushort) \
  MACRO(unsigned int, uint) \
  MACRO(unsigned long, ulong) \
  MACRO(unsigned long long, ulonglong)

/**
 * @ingroup header_data_classes
 * @brief The variant value type for records.
 * @warning
 * This is a boost::any today, which should be replaced with a boost::variant at some point,
 * thus the need for this alias.
 */
using VariantValue = boost::any;

/**
 * @ingroup header_data_classes
 * @brief Keyword-value pair with optional unit and comment.
 * @tparam T The value type,
 * which can be an integer, floating point, complex, `std::string`, `const char *`,
 * or `VariantValue` for run-time type deduction.
 *
 * @details
 * Here's a record of type `double` with keyword `"LIGHT"`, value `3.0e8`, unit `"m/s"` and comment `"speed of light"`:
 * \code
 * Record<double> lightSpeed {"LIGHT", 3.0e8, "m/s", "speed of light"};
 * \endcode
 *
 * In the FITS file, this record will appear in the header of an HDU as (padding blank spaces removed):
 * \code
 * LIGHT = 3.0E8 / [m/s] speed of light
 * \endcode
 *
 * In the FITS definition, it is unclear if the "comment" encompasses only: `speed of light`, or also the unit, as: `[m/s] speed of light`.
 * In EleFits, the former is named comment, while the latter is the raw comment.
 * The raw comment can be get as `Record::rawComment()`.
 * 
 * Such a `Record` can be cast to `double` (records of value type `T` can be cast to `T`),
 * or more precisely, it can be sliced as its value.
 * \code
 * double milleniumFalconSpeed = 1.5 * lightSpeed;
 * // Same as: 1.5 * lightSpeed.value
 * \endcode
 *
 * This is also usefull when aiming at reading record values only, and skip the keyword, unit and comment:
 * \code
 * Record<int> theRecord = header.read<int>("KEYWORD");
 * int theValue = header.read<int>("KEYWORD");
 * \endcode
 *
 * The "HIERARCH" convention for extended keywords is supported.
 * It occurs when the keyword is longer than 8 characters,
 * or contains non-standard characters like spaces or symbols.
 * Such records are read and written transparently as:
 * \code
 * HIERARCH the_long_keyword = value / [unit] comment
 * \endcode
 * The maximum length of such a keyword is 67 characters, which gives room for a 1-byte long value.
 *
 * CFITSIO convention on long string values (more than 68 characters) is supported.
 * When writing a long string record, the value is wrapped automatically,
 * and each new line starts with the CONTINUE keyword.
 * An additional "LONGSTRN" record is written to the file,
 * to warn the file user about the CFITSIO convention.
 */
template <typename T>
struct Record {

  /**
   * @brief The value type.
   */
  using Value = T;

  /**
   * @brief Constructor.
   * @param k The keyword.
   * @param v The value.
   * @param u The unit.
   * @param c The comment.
   */
  Record(const std::string& k = "", T v = T(), const std::string& u = "", const std::string& c = "");

  /**
   * @brief Create a record from a Record of another type.
   * @details
   * This constructor can be used to homogenize types, for example to create a
   * `vector<Record<any>>` from various `Record<T>`s with different `T`s.
   * @warning
   * Source type TFrom must be castable to destination type T.
   * @see cast
   */
  template <typename TFrom>
  explicit Record(const Record<TFrom>& other);

  /**
   * @brief Assign new fields to the record.
   */
  template <typename TFrom>
  Record<T>& assign(const std::string& k = "", TFrom v = TFrom(), const std::string& u = "", const std::string& c = "");

  /**
   * @brief Copy a Record of another type.
   */
  template <typename U>
  Record<T>& assign(const Record<U>& other);

  /**
   * @brief Helper function to cast Record value types.
   * Valid casts are:
   * - scalar number -> scalar number
   * - complex number -> complex number
   * - `any` -> scalar number if the underlying value type is a scalar number
   * - `any` -> complex number if the value type is a complex number
   * - `any` -> `string` if the value type is a `string`
   * - scalar number -> `any`
   * - complex number -> `any`
   * - `string` -> `any`
   */
  template <typename TFrom>
  static T cast(TFrom value);

  /**
   * @brief Slice the record as its value.
   * @details
   * A Record is often read for using its value only;
   * This shortcut is merely a const version of the value member.
   * \code
   * SifFile f("filename.fits");
   * const auto& h = f.header();
   * // Immediately cast to int:
   * int value = h.parseRecord<int>("KEYWORD");
   * // Same as:
   * int value = h.parseRecord<int>("KEYWORD").value;
   * \endcode
   */
  operator T() const;

  /**
   * @brief Get the raw comment string.
   * @details
   * When there is a unit, the raw comment is: "[unit] comment".
   * When unit is empty, the raw comment is "comment".
   */
  std::string rawComment() const;

  /**
   * @brief Check whether the keyword of a record is long string (more than 8 characters).
   * @details
   * A long-keyword record follows the hierarchical keyword convention.
   * @see Record documentation for more details on the hierarchical keyword convention.
   */
  bool hasLongKeyword() const;

  /**
   * @brief Check whether the value of a record is a long string (more than 68 characters).
   * @return True if the record value is a string (see details) and its length is greater than 68;
   * False otherwise.
   * @details
   * A long string value is written on several lines, using the CONTINUE special keyword.
   * The method is provided for any type for convenience, but always returns false for non-string values.
   * Are considered string types:
   * * `std::string`,
   * * `const char *`,
   * * `VariantValue` if the underlying value is one of the previous types.
   * @see Record documentation for more details on the long string value convention.
   */
  bool hasLongStringValue() const;

  /**
   * @brief The keyword.
   */
  std::string keyword;

  /**
   * @brief The value.
   */
  T value;

  /**
   * @brief The unit.
   */
  std::string unit;

  /**
   * @brief The comment without the unit.
   */
  std::string comment;
};

/**
 * @relates Record
 * @brief Check whether two records are equal.
 * @details
 * Equality of keyword, value, unit and comment is tested.
 */
template <typename T>
bool operator==(const Record<T>& lhs, const Record<T>& rhs) {
  if (lhs.keyword != rhs.keyword) {
    return false;
  }
  if (lhs.value != rhs.value) {
    return false;
  }
  if (lhs.unit != rhs.unit) {
    return false;
  }
  if (lhs.comment != rhs.comment) {
    return false;
  }
  return true;
}

/**
 * @relates Record
 * @brief Check whether two records are different.
 */
template <typename T>
bool operator!=(const Record<T>& lhs, const Record<T>& rhs) {
  return not(lhs == rhs);
}

/**
 * @relates Record
 * @brief Serialize a record.
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Record<T>& r) {
  os << r.keyword << " = " << r.value;
  const auto rc = r.rawComment();
  if (not rc.empty()) {
    os << " / " << rc;
  }
  return os;
}

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_RECORD_IMPL
#include "EleFitsData/impl/Record.hpp"
#undef _ELEFITSDATA_RECORD_IMPL
/// @endcond

#endif
