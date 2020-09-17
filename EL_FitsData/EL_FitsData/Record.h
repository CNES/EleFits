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

#ifndef _EL_FITSDATA_RECORD_H
#define _EL_FITSDATA_RECORD_H

#include <string>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Keyword-value pair with optional unit and comment.
 * @tparam T The value type;
 * Can be an integer, floating point, complex, \c std::string or \c boost::any.
 * @details
 * Record is meant to be used to read and write Fits headers.
 * The Record in the Fits file is rendered as:
 * \verbatim keyword = value / [unit] comment \endverbatim
 * if the unit is provided, or:
 * \verbatim keyword = value / comment \endverbatim
 * otherwise.
 * @see \ref data-classes
 */
template <typename T>
struct Record {

  /**
   * @brief Assign a record.
   * @param k The keyword
   * @param v The value
   * @param u The unit
   * @param c The comment
   */
  Record(const std::string &k = "", T v = T(), const std::string &u = "", const std::string &c = "");

  /**
   * @brief Create a Record from a Record of another type.
   * @details
   * This constructor can be used to homogeneize types, for example to create a
   * \c vector<Record<any>> from various \c Record<T>'s with different \c T's.
   * @warning
   * Source type TOther must be castable to destination type T.
   * @see cast
   */
  template <typename TOther>
  Record(const Record<TOther> &other);

  /**
   * @brief Helper function to cast Record value types.
   * Valid casts are:
   * - scalar number -> scalar number
   * - complex number -> complex number
   * - \c any -> scalar number if the underlying value type is a scalar number
   * - \c any -> complex number if the value type is a complex number
   * - \c any -> \c string if the value type is a \c string
   * - scalar number -> \c any
   * - complex number -> \c any
   * - \c string -> \c any
   */
  template <typename TOther>
  static T cast(TOther value);

  /**
   * @brief Slice the record as its value.
   * @details
   * A Record is often read for using its value only;
   * This shortcut is merely a const version of the value member.
   * @code
   * SifFile f("filename.fits");
   * const auto& h = f.header();
   * // Immediately cast to int:
   * int value = h.parseRecord<int>("KEYWORD");
   * // Same as:
   * int value = h.parseRecord<int>("KEYWORD").value;
   * @endcode
   */
  operator T() const;

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

} // namespace FitsIO
} // namespace Euclid

#include "impl/Record.hpp"

#endif
