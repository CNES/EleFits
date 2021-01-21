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

#ifndef _EL_FITSDATA_DATAUTILS_H
#define _EL_FITSDATA_DATAUTILS_H

namespace Euclid {
namespace FitsIO {

/**
 * @brief A name and type.
 */
template <typename T>
struct Named {

  /** @brief Constructor. */
  Named(std::string value) : name(value) {};

  /**
   * @brief Slice as the name.
   */
  operator std::string() const {
    return name;
  }

  /** @brief The name */
  std::string name;
};

/**
 * @brief An index and type.
 */
template <typename T>
struct Indexed {

  /** @brief Constructor. */
  Indexed(long value) : index(value) {};

  /**
   * @brief Slice as the index.
   */
  operator long() const {
    return index;
  }

  /** @brief The index */
  long index;
};

} // namespace FitsIO

} // namespace Euclid

#endif
