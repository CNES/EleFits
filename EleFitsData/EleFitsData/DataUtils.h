// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_DATAUTILS_H
#define _ELEFITSDATA_DATAUTILS_H

#include <algorithm> // transform
#include <array>
#include <limits>
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
 * @brief Get the `BITPIX` value of a given type.
 */
template <typename T>
constexpr long bitpix() {
  if constexpr (std::is_integral_v<T>) {
    return 8 * static_cast<int>(sizeof(T));
  }
  if constexpr (std::is_floating_point_v<T>) {
    return -8 * static_cast<int>(sizeof(T));
  }
  return 0;
}

/// @cond
template <typename T>
struct ChangeSignednessImpl {
  using Type = std::conditional_t<std::is_signed_v<T>, std::make_unsigned_t<T>, std::make_signed_t<T>>;
};

template <>
struct ChangeSignednessImpl<float> {
  using Type = float;
};

template <>
struct ChangeSignednessImpl<double> {
  using Type = double;
};
/// @endcond

/**
 * @brief Inverse the signedness of integers, do nothing for floating points.
 */
template <typename T>
using ChangeSignedness = typename ChangeSignednessImpl<T>::Type;

/**
 * @brief Get the opposite of the `BZERO` value of a given type.
 */
template <typename T>
constexpr ChangeSignedness<T> offset() {
  if constexpr (std::is_floating_point_v<T>) {
    return 0;
  } else if constexpr (sizeof(T) == 1) {
    if constexpr (std::is_signed_v<T>) {
      return 128;
    } else {
      return 0;
    }
  } else if constexpr (std::is_signed_v<T>) {
    return 0;
  } else {
    return std::numeric_limits<std::make_signed_t<T>>::min();
  }
}

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

  /**
   * @brief The return type.
   */
  using Return = TReturn;

  /**
   * @brief The key type.
   */
  using Key = TKey;

  /**
   * @brief Constructor.
   * @see as()
   */
  explicit TypedKey(TKey k) : key(k) {}

  /**
   * @brief The key.
   */
  TKey key;
};

/**
 * @relates TypedKey
 * @brief Create a `TypedKey` where the key type is deduced from the parameter.
 * @details
 * \par Example
 * \code
 * columns.readSeq(as<float>(1), as<std::string>(4));
 * \endcode
 */
template <typename TReturn>
TypedKey<TReturn, long> as(long key) {
  return TypedKey<TReturn, long> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, long> as(int key) {
  return TypedKey<TReturn, long> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, std::string> as(const std::string& key) {
  return TypedKey<TReturn, std::string> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, std::string> as(const char* key) {
  return TypedKey<TReturn, std::string> {key};
}

/// @cond
namespace Internal {

/**
 * @brief Make an index sequence for a tuple.
 */
template <typename TTuple>
constexpr decltype(auto) tuple_index_sequence() {
  return std::make_index_sequence<std::tuple_size<std::remove_reference_t<TTuple>>::value>();
}

/**
 * @brief Convert a tuple to a user-defined struct.
 */
template <typename TReturn, typename TTuple, std::size_t... Is>
TReturn tuple_as_impl(TTuple&& tuple, std::index_sequence<Is...>) {
  return {std::get<Is>(tuple)...};
}

/**
 * @brief Apply a variadic function to a tuple.
 */
template <typename TTuple, typename TFunc, std::size_t... Is>
constexpr decltype(auto) apply_impl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  return func(std::get<Is>(tuple)...);
}

/**
 * @brief Apply a variadic function to elements pointed by a tuple of iterators
 * and increment the iterators.
 */
template <typename TIteratorTuple, typename TFunc, std::size_t... Is>
constexpr decltype(auto) iterator_tuple_apply_impl(TIteratorTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  return func(*std::get<Is>(tuple)++...);
}

/**
 * @brief Apply a function which returns void to each element of a tuple.
 */
template <typename TTuple, typename TFunc, std::size_t... Is>
void tuple_foreach_impl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
  using mock_unpack = int[];
  (void)mock_unpack {
      0, // Ensure there is at least one element
      (func(std::get<Is>(tuple)), // Use comma operator to return an int even if func doesn't
       void(), // Add void() in case where the return type of func would define a comma-operator
       0)...};
}

/**
 * @brief Apply a function to each element of a tuple, and make a user-defined struct from the results.
 */
template <typename TReturn, typename TTuple, typename TFunc, std::size_t... Is>
TReturn tuple_transform_impl(TTuple&& tuple, TFunc&& func, std::index_sequence<Is...>) {
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
constexpr bool is_tuple() {
  return Internal::IsTupleImpl<std::decay_t<TSeq>>::value;
}

/**
 * @brief Convert a tuple to a custom structure.
 */
template <typename TReturn, typename TTuple>
TReturn tuple_as(TTuple&& tuple) {
  return Internal::tuple_as_impl<TReturn>(std::forward<TTuple>(tuple), Internal::tuple_index_sequence<TTuple>());
}

/**
 * @brief Apply a variadic function a tuple.
 */
template <typename TTuple, typename TFunc>
constexpr decltype(auto) tuple_apply(TTuple&& tuple, TFunc&& func) {
  return Internal::apply_impl(
      std::forward<TTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tuple_index_sequence<TTuple>());
}

/**
 * @brief Apply a variadic function to elements pointed by a tuple of iterators
 * and increment the iterators.
 */
template <typename TIteratorTuple, typename TFunc>
constexpr decltype(auto) iterator_tuple_apply(TIteratorTuple&& tuple, TFunc&& func) {
  return Internal::iterator_tuple_apply_impl(
      std::forward<TIteratorTuple>(tuple),
      std::forward<TFunc>(func),
      Internal::tuple_index_sequence<TIteratorTuple>());
}

/**
 * @brief Apply a void-returning function to each element of a sequence.
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<is_tuple<TSeq>()> seq_foreach(TSeq&& seq, TFunc&& func) {
  Internal::tuple_foreach_impl(
      std::forward<TSeq>(seq),
      std::forward<TFunc>(func),
      Internal::tuple_index_sequence<TSeq>());
}

/**
 * @copydoc seq_foreach()
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<not is_tuple<TSeq>()> seq_foreach(const TSeq& seq, TFunc&& func) {
  for (const auto& element : seq) {
    func(element);
  }
}

/**
 * @copydoc seq_foreach()
 */
template <typename TSeq, typename TFunc>
std::enable_if_t<not is_tuple<TSeq>()> seq_foreach(TSeq& seq, TFunc&& func) {
  for (auto& element : seq) {
    func(element);
  }
}

/**
 * @brief Apply a transform to each element of a sequence and create a user-defined struct from the results.
 */
template <typename TReturn, typename TSeq, typename TFunc>
std::enable_if_t<is_tuple<TSeq>(), TReturn> seq_transform(TSeq&& seq, TFunc&& func) {
  return Internal::tuple_transform_impl<TReturn>(
      std::forward<TSeq>(seq),
      std::forward<TFunc>(func),
      Internal::tuple_index_sequence<TSeq>());
}

/**
 * @copydoc seq_transform()
 */
template <typename TReturn, typename TSeq, typename TFunc>
std::enable_if_t<not is_tuple<TSeq>(), TReturn> seq_transform(const TSeq& seq, TFunc&& func) {
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
 * log_args(std::cout, 1, 3.14, "str");
 * \endcode
 * Prints: `1, 3.14, str`
 */
template <typename TLogger, typename T0, typename... Ts>
void log_args(TLogger&& logger, T0&& arg0, Ts&&... args) {
  logger << std::forward<T0>(arg0);
  using mock_unpack = int[];
  (void)mock_unpack {0, (void(std::forward<TLogger>(logger) << ", " << std::forward<Ts>(args)), 0)...};
}

} // namespace Fits
} // namespace Euclid

#endif
