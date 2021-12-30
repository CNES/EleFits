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

#ifndef _ELEFITSDATA_VECTORARITHMETIC_H
#define _ELEFITSDATA_VECTORARITHMETIC_H

#include <algorithm>
#include <boost/operators.hpp>
#include <functional>
#include <type_traits>

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_classes
 * @brief Mixin to provide vector space arithmetics to a container.
 * @details
 * Implemented arithmetic properties:
 * - Vector-additive: V += U, W = V + U, V += U, W = V - U;
 * - Scalar-additive: V += a, V = U + a, V = a + U, V -= a, V = U + a, V = a - U;
 * - Scalar-multiplicative: V *= a, V = U * a, V = a * U, V /= a, V = U / a;
 * - Incrementation if enabled (for integral types by default): V++, ++V, V--, --V.
 */
template <typename T, typename TDerived, bool Incrementable = std::is_integral<T>::value>
struct VectorArithmeticMixin;

/**
 * @copydoc VectorArithmeticMixin
 */
template <typename T, typename TDerived>
struct VectorArithmeticMixin<T, TDerived, false> :
    boost::additive<TDerived>,
    boost::additive<TDerived, T>,
    boost::subtractable2_left<TDerived, T>,
    boost::multiplicative<TDerived, T> {

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

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator+=(const TDerived& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      rhs.begin(),
      static_cast<TDerived*>(this)->begin(),
      std::plus<>());
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator+=(const T& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [&](auto lhs) {
        return lhs + rhs;
      });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator-=(const TDerived& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      rhs.begin(),
      static_cast<TDerived*>(this)->begin(),
      std::minus<>());
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator-=(const T& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [&](auto lhs) {
        return lhs - rhs;
      });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator*=(const T& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [&](auto lhs) {
        return lhs * rhs;
      });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, false>::operator/=(const T& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [&](auto lhs) {
        return lhs / rhs;
      });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, true>::operator++() {
  std::transform(TDerived::begin(), TDerived::end(), TDerived::begin(), [](auto rhs) {
    return ++rhs;
  });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, true>::operator--() {
  std::transform(TDerived::begin(), TDerived::end(), TDerived::begin(), [](auto rhs) {
    return --rhs;
  });
  return static_cast<TDerived&>(*this);
}

} // namespace Fits
} // namespace Euclid

#endif
