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

#include <boost/any.hpp>
#include <string>
#include <tuple>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Keyword-value pair with optional unit and comment.
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
   * Destination type T must be constructible from source type TOther.
   * This constructor can be used to homogeneize types, for example to create a
   * \c vector<Record<any>> from various \c Record<T>'s with different \c T's.
   */
  template <typename TOther>
  Record(const Record<TOther> &other);

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

/////////////////
/// INTERNAL ///
///////////////

namespace Internal {

template <typename TOther>
struct CasterImpl {
  template <typename T>
  static T cast(TOther value);
};

template <typename TOther>
template <typename T>
T CasterImpl<TOther>::cast(TOther value) {
  return T(value);
}

template <>
template <typename T>
T CasterImpl<boost::any>::cast(boost::any value);

template <>
template <typename T>
T CasterImpl<boost::any>::cast(boost::any value) {
  return boost::any_cast<T>(value);
}

} // namespace Internal

/////////////////////
// IMPLEMENTATION //
///////////////////

template <typename T>
Record<T>::Record(const std::string &k, T v, const std::string &u, const std::string &c) :
    keyword(k),
    value(v),
    unit(u),
    comment(c) {
}

template <typename T>
template <typename TOther>
Record<T>::Record(const Record<TOther> &other) :
    keyword(other.keyword),
    value(Internal::CasterImpl<TOther>::template cast<T>(other.value)),
    unit(other.unit),
    comment(other.comment) {
}

template <typename T>
Record<T>::operator T() const {
  return value;
}

} // namespace FitsIO
} // namespace Euclid

#endif
