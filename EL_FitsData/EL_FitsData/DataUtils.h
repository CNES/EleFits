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

/**
 * @brief Make an index sequence for a tuple.
 */
template <typename TTuple>
constexpr decltype(auto) tupleIndexSequence() {
  return std::make_index_sequence<std::tuple_size<std::remove_reference_t<TTuple>>::value>();
}

/**
 * @brief Convert a tuple to a user-defined struct.
 */
template <typename TReturn, typename TTuple, std::size_t... Is>
TReturn tupleAsImpl(TTuple&& tuple, std::index_sequence<Is...>) {
  return { std::get<Is>(tuple)... };
}

/**
 * @brief Apply a variadic function to a tuple.
 */
template <typename TTuple, typename TFunc, std::size_t... Is>
constexpr decltype(auto) applyImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  return func(std::get<Is>(tuple)...);
}

/**
 * @brief Apply a function which returns void to each element of a tuple.
 */
template <typename TTuple, typename TFunc, std::size_t... Is>
void foreachImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  using mockUnpack = int[];
  (void)mockUnpack { 0, // Ensure there is at least one element
                     (func(std::get<Is>(tuple)), // Use comma operator to return an int even if func doesn't
                      void(), // Add void() in case where the return type of func would define a comma-operator
                      0)... };
}

/**
 * @brief Apply a function to each element of a tuple, and make a user-defined struct from the results.
 */
template <typename TReturn, typename TTuple, typename TFunc, std::size_t... Is>
TReturn transformImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  return { func(std::get<Is>(tuple))... };
}

} // namespace Internal

/**
 * @brief Convert a tuple to a custom structure.
 */
template <typename TReturn, typename TTuple>
TReturn tupleAs(TTuple&& tuple) {
  return Internal::tupleAsImpl<TReturn>(std::forward<TTuple>(tuple), Internal::tupleIndexSequence<TTuple>());
}

/**
   * @brief Apply a variadic function a tuple.
   */
template <typename TTuple, typename TFunc>
constexpr decltype(auto) tupleApply(TTuple&& tuple, TFunc&& func) {
  return Internal::applyImpl(
      std::forward<TTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tupleIndexSequence<TTuple>());
}

/**
   * @brief Apply a void-returning function to each element of the tuple.
   */
template <typename TTuple, typename TFunc>
void tupleForeach(TTuple&& tuple, TFunc&& func) {
  Internal::foreachImpl(std::forward<TTuple>(tuple), std::forward<TFunc>(func), Internal::tupleIndexSequence<TTuple>());
}

/**
   * @brief Apply a transform to each element of the tuple and make a user-defined struct from the results.
   */
template <typename TReturn, typename TTuple, typename TFunc>
TReturn tupleTransform(TTuple&& tuple, TFunc&& func) {
  return Internal::transformImpl<TReturn>(
      std::forward<TTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tupleIndexSequence<TTuple>());
}

} // namespace FitsIO
} // namespace Euclid

#endif
