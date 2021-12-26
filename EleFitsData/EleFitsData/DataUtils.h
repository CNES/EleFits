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

#ifndef _ELEFITSDATA_DATAUTILS_H
#define _ELEFITSDATA_DATAUTILS_H

#include <algorithm> // transform
#include <array>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @brief Define a default virtual destructor.
 */
#define ELEFITS_VIRTUAL_DTOR(classname) \
  /** @brief Destructor. */ \
  virtual ~classname() = default;

/**
 * @brief Define default copy constructor and assignment operator.
 */
#define ELEFITS_COPYABLE(classname) \
  /** @brief Copy constructor. */ \
  classname(const classname&) = default; \
  /** @brief Copy assignment operator. */ \
  classname& operator=(const classname&) = default;

/**
 * @brief Define deleted copy constructor and assignment operator.
 */
#define ELEFITS_NON_COPYABLE(classname) \
  /** @brief Deleted copy constructor. */ \
  classname(const classname&) = delete; \
  /** @brief Deleted copy assignment operator. */ \
  classname& operator=(const classname&) = delete;

/**
 * @brief Define default move constructor and assignment operator.
 */
#define ELEFITS_MOVABLE(classname) \
  /** @brief Move constructor. */ \
  classname(classname&&) = default; \
  /** @brief Move assignment operator. */ \
  classname& operator=(classname&&) = default;

/**
 * @brief Define deleted move constructor and assignment operator.
 */
#define ELEFITS_NON_MOVABLE(classname) \
  /** @brief Deleted move constructor. */ \
  classname(classname&&) = delete; \
  /** @brief Deleted move assignment operator. */ \
  classname& operator=(classname&&) = delete;

/**
 * @ingroup data_classes
 * @brief A light structure to bind a return type and a key, e.g. for reading records and columns.
 * @tparam TReturn The desired return type
 * @tparam TKey The key type, typically `std::string` or `long`
 * @details
 * `TypedKeys` should not be instantiated directly.
 * Instead maker `as()` should be used for clarity, e.g.:
 * \code
 * // Ugly
 * header.parseSeq(TypedKey<float, std::string>("A"), TypedKey<int, std::string>("B"));
 * 
 * // Clean
 * header.parseSeq(as<float>("A"), as<int>("B"));
 * \endcode
 */
template <typename TReturn, typename TKey>
struct TypedKey {
  using Return = TReturn;
  using Key = TKey;
  explicit TypedKey(TKey k) : key(k) {}
  TKey key;
};

/**
 * @brief Deprecated alias for named keys.
 * @deprecated Use `as()` instead.
 */
template <typename TReturn>
using Named = TypedKey<TReturn, std::string>;

/**
 * @brief Deprecated alias for indexed keys.
 * @deprecated Use `as()` instead.
 */
template <typename TReturn>
using Indexed = TypedKey<TReturn, long>;

/**
 * @brief Create a `TypedKey` where the key type is deduced from the parameter.
 * @see TypedKey
 */
template <typename TReturn>
TypedKey<TReturn, long> as(long key) {
  return TypedKey<TReturn, long> {key};
}
template <typename TReturn>
TypedKey<TReturn, long> as(int key) {
  return TypedKey<TReturn, long> {key};
}
template <typename TReturn>
TypedKey<TReturn, std::string> as(const std::string& key) {
  return TypedKey<TReturn, std::string> {key};
}
template <typename TReturn>
TypedKey<TReturn, std::string> as(const char* key) {
  return TypedKey<TReturn, std::string> {key};
}

/**
 * @ingroup bintable_data_classes
 * @brief Bounds of a closed index interval.
 * @details
 * The lower and upper bounds are named `front` and `back`
 * to match `Region` wording.
 * This also avoids confusion when working with table segments,
 * where the lower bound is upward the upper bound.
 */
struct Segment {

  /**
   * @brief Create a segment specified by a lower bound and size.
   */
  static Segment fromSize(long front, long size) {
    return {front, front + size - 1};
  }

  /**
   * @brief Create a maximal segment (from index 0 to max).
   */
  static Segment whole() {
    return {0, -1};
  }

  /**
   * @brief The lower bound.
   */
  long front;

  /**
   * @brief The upper bound.
   */
  long back;

  /**
   * @brief Get the number of elements.
   */
  long size() const {
    return back - front + 1;
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
  return {std::get<Is>(tuple)...};
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
  (void)mockUnpack {
      0, // Ensure there is at least one element
      (func(std::get<Is>(tuple)), // Use comma operator to return an int even if func doesn't
       void(), // Add void() in case where the return type of func would define a comma-operator
       0)...};
}

/**
 * @brief Apply a function to each element of a tuple, and make a user-defined struct from the results.
 */
template <typename TReturn, typename TTuple, typename TFunc, std::size_t... Is>
TReturn tupleTransformImpl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  return {func(std::get<Is>(tuple))...};
}

/**
 * @brief Traits class to test wether a sequence is a tuple.
 * @details
 * Use `IsTupleImpl<T>::value` to get a `bool`.
 */
template <typename TSeq>
struct IsTupleImpl : std::false_type {};

/**
 * @copydoc IsTupleImpl
 */
template <typename... Ts>
struct IsTupleImpl<std::tuple<Ts...>> : std::true_type {};

/**
 * @copydoc IsTupleImpl
 */
template <typename T, std::size_t N>
struct IsTupleImpl<std::array<T, N>> : std::true_type {};

} // namespace Internal
/// @endcond

/**
 * @brief Test whether a sequence is a tuple.
 */
template <typename TSeq>
constexpr bool isTuple() {
  return Internal::IsTupleImpl<std::decay_t<TSeq>>::value;
}

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
 * @brief Apply a void-returning function to each element of a sequence.
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<isTuple<TSeq>()> seqForeach(TSeq&& seq, TFunc&& func) {
  Internal::tupleForeachImpl(std::forward<TSeq>(seq), std::forward<TFunc>(func), Internal::tupleIndexSequence<TSeq>());
}

/**
 * @copydoc seqForeach()
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<not isTuple<TSeq>()> seqForeach(const TSeq& seq, TFunc&& func) {
  for (const auto& element : seq) {
    func(element);
  }
}

/**
 * @copydoc seqForeach()
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<not isTuple<TSeq>()> seqForeach(TSeq& seq, TFunc&& func) {
  for (auto& element : seq) {
    func(element);
  }
}

/**
 * @brief Apply a transform to each element of a sequence and create a user-defined struct from the results.
 */
template <typename TReturn, typename TSeq, typename TFunc>
std::enable_if_t<isTuple<TSeq>(), TReturn> seqTransform(TSeq&& seq, TFunc&& func) {
  return Internal::tupleTransformImpl<TReturn>(
      std::forward<TSeq>(seq),
      std::forward<TFunc>(func),
      Internal::tupleIndexSequence<TSeq>());
}

/**
 * @copydoc seqTransform()
 */
template <typename TReturn, typename TSeq, typename TFunc>
std::enable_if_t<not isTuple<TSeq>(), TReturn> seqTransform(const TSeq& seq, TFunc&& func) {
  TReturn res(seq.size());
  std::transform(seq.begin(), seq.end(), res.begin(), std::forward<TFunc>(func));
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
  (void)mockUnpack {0, (void(std::forward<TLogger>(logger) << ", " << std::forward<Ts>(args)), 0)...};
}

} // namespace Fits
} // namespace Euclid

#endif
