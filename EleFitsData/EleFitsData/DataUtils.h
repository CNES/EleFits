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
 * @brief Get the `BITPIX` value of a given type.
 */
template <typename T>
constexpr long bitpix()
{
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
constexpr ChangeSignedness<T> offset()
{
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
 * header.parse_n(TypedKey<float, std::string>("A"), TypedKey<int, std::string>("B"));
 * 
 * // Clean
 * header.parse_n(as<float>("A"), as<int>("B"));
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
 * columns.read_n(as<float>(1), as<std::string>(4));
 * \endcode
 */
template <typename TReturn>
TypedKey<TReturn, long> as(long key)
{
  return TypedKey<TReturn, long> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, long> as(int key)
{
  return TypedKey<TReturn, long> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, std::string> as(const std::string& key)
{
  return TypedKey<TReturn, std::string> {key};
}

/**
 * @relates TypedKey
 * @copydoc as(long)
 */
template <typename TReturn>
TypedKey<TReturn, std::string> as(const char* key)
{
  return TypedKey<TReturn, std::string> {key};
}

} // namespace Fits
} // namespace Euclid

#endif
