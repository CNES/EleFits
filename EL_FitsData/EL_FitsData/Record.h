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
#include <tuple>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Keyword-value pair with optional unit and comment.
 * @details
 * If unit is provided, the comment will be rendered as "[unit] comment" in the Fits file.
 * @see \ref data-classes
 */
template<typename T>
struct Record {

  /**
   * @brief Assign a record.
   */
  Record(const std::string& k="", T v=T(), const std::string& u="", const std::string& c="");

  /**
   * @brief Slice the record as its value.
   */
  operator T () const;

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


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Record<T>::Record(const std::string& k, T v, const std::string& u, const std::string& c) :
  keyword(k),
  value(v),
  unit(u),
  comment(c) {}

template<typename T>
Record<T>::operator T() const {
  return value;
}

}
}

#endif
