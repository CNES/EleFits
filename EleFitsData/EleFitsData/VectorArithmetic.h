/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _ELEFITSDATA_VECTORARITHMETIC_H
#define _ELEFITSDATA_VECTORARITHMETIC_H

#include <algorithm>
#include <boost/operators.hpp>
#include <functional>
#include <type_traits>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_concepts
 * @requirements{VectorArithmetic}
 * @brief Vector-space arithmetic requirements.
 * @details
 * Implements vector space arithmetic operators
 * (uppercase letters are for vectors, lowercase letters are for scalars):
 * - Vector-additive: V += U, W = V + U, V += U, W = V - U;
 * - Scalar-additive: V += a, V = U + a, V = a + U, V -= a, V = U + a, V = a - U;
 * - Scalar-multiplicative: V *= a, V = U * a, V = a * U, V /= a, V = U / a;
 * - Incrementation if enabled (for integral value types by default): V++, ++V, V--, --V.
 */

/**
 * @ingroup data_concepts
 * @brief Mixin to provide vector space arithmetics to a container.
 * @tparam T The contained element value type
 * @tparam TDerived The container which inherits this class
 * @tparam Incrementable A flag to activate increment and decrement operators
 * @details
 * In addition to vector space arithmetic operators, this mixin provides
 * `generate()` and `apply()` to apply a function to each element.
 * @satisfies{VectorArithmetic}
 */
template <typename T, typename TDerived, bool Incrementable = std::is_integral<T>::value>
struct VectorArithmeticMixin :
    boost::additive<TDerived>,
    boost::additive<TDerived, T>,
    boost::subtractable2_left<TDerived, T>,
    boost::multiplicative<TDerived, T> {

  /// @group_modifiers

  /**
   * @brief V += U and W = V + U.
   */
  TDerived& operator+=(const TDerived& rhs);

  /**
   * @brief V += a, V = U + a, V = a + U.
   */
  TDerived& operator+=(const T& rhs);

  /**
   * @brief V -= U and W = V - U.
   */
  TDerived& operator-=(const TDerived& rhs);

  /**
   * @brief V -= a, V = U - a, V = a - U.
   */
  TDerived& operator-=(const T& rhs);

  /**
   * @brief V *= a, V = U * a, V = a * U.
   */
  TDerived& operator*=(const T& rhs);

  /**
   * @brief V /= a, V = U / a.
   */
  TDerived& operator/=(const T& rhs);

  /**
   * @brief Generate values from a function with optional input containers.
   * @param func The generator function, which takes as many inputs as there are arguments
   * @param args The arguments in the form of containers of compatible sizes
   * @details
   * For example, here is how to imlement element-wise square root and multiplication:
   * \code
   * Container a = ...;
   * Container b = ...;
   * Container res(a.size());
   * res.generate([](auto v) { return std::sqrt(v) }, a); // res = sqrt(a)
   * res.generate([](auto v, auto w) { return v * w; }, a, b); // res = a * b
   * \endcode
   */
  template <typename TFunc, typename... TContainers>
  TDerived& generate(TFunc&& func, const TContainers&... args);

  /**
   * @brief Apply a function with optional input containers.
   * @param func The function
   * @param args The arguments in the form of containers of compatible sizes
   * @details
   * If there are _n_ arguments, `func` takes _n_+1 parameters,
   * where the first argument is the element of this container.
   * For example, here is how to imlement in-place element-wise square root and multiplication:
   * \code
   * Container a = ...;
   * Container res = ...;
   * res.apply([](auto v) { return std::sqrt(v); }); // res = sqrt(res)
   * res.apply([](auto v, auto w) { return v * w; }, a); // res *= a
   * \endcode
   */
  template <typename TFunc, typename... TContainers>
  TDerived& apply(TFunc&& func, const TContainers&... args);

  /// @group_operations

  /**
   * @brief Copy.
   */
  TDerived operator+() const;

  /**
   * @brief Compute the opposite.
   */
  TDerived operator-() const;

  /// @}
};

/**
 * @copydoc VectorArithmeticMixin
 */
template <typename T, typename TDerived>
struct VectorArithmeticMixin<T, TDerived, true> :
    VectorArithmeticMixin<T, TDerived, false>,
    boost::unit_steppable<TDerived> {

  /**
   * @brief ++V and V++.
   */
  TDerived& operator++();

  /**
   * @brief --V and V--.
   */
  TDerived& operator--();
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_VECTORARITHMETIC_IMPL
#include "EleFitsData/impl/VectorArithmetic.hpp"
#undef _ELEFITSDATA_VECTORARITHMETIC_IMPL
/// @endcond

#endif
