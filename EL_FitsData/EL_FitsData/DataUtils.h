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

#include <string>
#include <tuple>

namespace Euclid {
namespace FitsIO {

/**
 * @brief A name and type.
 */
template <typename T>
struct Named {

  /** @brief Constructor. */
  Named(const std::string& value) : name(value) {};

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

/// @cond INTERNAL
namespace Internal {
template <typename TReturn, typename... Ts, std::size_t... Is>
TReturn tupleAsImpl(const std::tuple<Ts...>& elements, std::index_sequence<Is...>) {
  return { std::get<Is>(elements)... };
}
} // namespace Internal

/**
 * @brief A tuple wrapper to homogeneize signatures.
 * @
 */
template <typename... Ts>
struct Tuple {

  /**
   * @brief Convert the series as a given type.
   * @tparam TReturn The return type, constructed from a brace-enclosed list of the elements
   */
  template <typename TReturn>
  TReturn as() const {
    return Internal::tupleAsImpl<TReturn>(tuple, std::make_index_sequence<sizeof...(Ts)>());
  }

  /**
   * @brief The elements as a tuple.
   */
  std::tuple<Ts...> tuple;
};

/**
 * @brief A series of names.
 */
template <typename... Ts>
using NameTuple = Tuple<Named<Ts>...>;

/**
 * @brief A series of indices.
 */
template <typename... Ts>
using IndexTuple = Tuple<Indexed<Ts>...>;

} // namespace FitsIO
} // namespace Euclid

#endif
