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

#include <algorithm> // transform
#include <string>
#include <tuple>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @brief A name and type.
 */
template <typename T>
struct Named {

  /**
   * @brief Constructor.
   */
  explicit Named(const std::string& value) : name(value) {};

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

  /**
   * @brief Constructor.
   */
  explicit Indexed(long value) : index(value) {};

  /**
   * @brief Slice as the index.
   */
  operator long() const {
    return index;
  }

  /** @brief The index */
  long index;
};

/**
 * @brief Bounds of a closed index interval.
 * @details
 * The lower and upper bounds are named `first` and `last`,
 * which is more natural when working with table segments,
 * where the first row is upward the last row.
 */
struct Segment {

  /**
   * @brief Create a segment specified by a lower bound and size.
   */
  static Segment fromSize(long first, long size) {
    return Segment { first, first + size - 1 };
  }

  /**
   * @brief The lower bound.
   */
  long first;

  /**
   * @brief The upper bound.
   */
  long last;

  /**
   * @brief Get the number of elements.
   */
  long size() const {
    return last - first + 1;
  }
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
void tupleForeachImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
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
TReturn tupleTransformImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
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
void seqForeach(TTuple&& tuple, TFunc&& func);

template <typename T, typename TFunc>
void seqForeach(const std::vector<T>& vector, TFunc&& func);

template <typename T, typename TFunc>
void seqForeach(std::vector<T>&& vector, TFunc&& func);

// FIXME use enable_if to dispatch based on the presence of get() or begin()

template <typename T>
class RecordVector; // FIXME rm

template <typename T, typename TFunc>
void seqForeach(const RecordVector<T>& vector, TFunc&& func);

template <typename T, typename TFunc>
void seqForeach(RecordVector<T>&& vector, TFunc&& func);

template <typename TTuple, typename TFunc>
void seqForeach(TTuple&& tuple, TFunc&& func) {
  Internal::tupleForeachImpl(
      std::forward<TTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tupleIndexSequence<TTuple>());
}

template <typename T, typename TFunc>
void seqForeach(const std::vector<T>& vector, TFunc&& func) {
  for (const auto& element : vector) {
    func(element);
  }
}

template <typename T, typename TFunc>
void seqForeach(std::vector<T>&& vector, TFunc&& func) {
  for (auto& element : vector) {
    func(element);
  }
}

template <typename T, typename TFunc>
void seqForeach(const RecordVector<T>& vector, TFunc&& func) {
  for (const auto& element : vector) {
    func(element);
  }
}

template <typename T, typename TFunc>
void seqForeach(RecordVector<T>&& vector, TFunc&& func) {
  for (auto& element : vector) {
    func(element);
  }
}

/**
 * @brief Apply a transform to each element of a tuple and create a user-defined struct from the results.
 */
template <typename TReturn, typename TTuple, typename TFunc>
TReturn seqTransform(TTuple&& tuple, TFunc&& func) {
  return Internal::tupleTransformImpl<TReturn>(
      std::forward<TTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tupleIndexSequence<TTuple>());
}

template <typename TReturn, typename T, typename TFunc>
TReturn seqTransform(const std::vector<T>& vector, TFunc&& func) {
  TReturn res(vector.size);
  std::transform(vector.begin(), vector.end(), res.begin(), func);
  return res;
}

template <typename TReturn, typename T, typename TFunc>
TReturn seqTransform(const RecordVector<T>& vector, TFunc&& func) {
  TReturn res(vector.size);
  std::transform(std::begin(vector), std::end(vector), std::begin(res), func);
  return res;
}

/**
 * @brief Log a heterogeneous list of arguments.
 * @details
 * Applies operator<<() to arguments, separated with ", ".
 * For example:
 * \code
 * logArgs(std::cout, 1, 3.14, "str");
 * \endcode
 * Prints: `1, 3.14, str`
 */
template <typename TLogger, typename T0, typename... Ts>
void logArgs(TLogger&& logger, T0&& arg0, Ts&&... args) {
  logger << std::forward<T0>(arg0);
  using mockUnpack = int[];
  (void)mockUnpack { 0, (void(std::forward<TLogger>(logger) << ", " << std::forward<Ts>(args)), 0)... };
}

} // namespace FitsIO
} // namespace Euclid

#endif
