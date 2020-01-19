/**
 * @file EL_FitsData/Record.h
 * @date 09/03/19
 * @author user
 *
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
 */
template<typename T>
struct Record {

  /**
   * @brief Assign a record.
   */
  Record(std::string k="", T v=T(), std::string u="", std::string c="");

  /**
   * @brief Slice the record as its value.
   */
  operator T () const;

  std::string keyword;
  T value;
  std::string unit; //TODO remove?
  std::string comment;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Record<T>::Record(std::string k, T v, std::string u, std::string c) :
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
