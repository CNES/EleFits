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

namespace Euclid {
namespace Fits {

/**
 * @ingroup data_classes
 * @brief Mixin to provide vector and scalar arithmetic operators.
 * @details
 * Implemented arithmetic properties:
 * - Vector-additive (V += U, W = V + U, V += U, W = V - U);
 * - Scalar-additive (V += a, V = U + a, V -= a, V = U + a);
 * - Scalar-multiplicative (V *= a, V = U * a, V /= a, V = U / a).
 */
template <typename T, typename TDerived>
struct VectorArithmeticMixin :
    boost::additive<TDerived>,
    boost::additive<TDerived, T>,
    boost::multiplicative<TDerived, T> {

  /**
   * @brief V += U and W = V + U.
   */
  TDerived& operator+=(const TDerived& rhs);

  /**
   * @brief V += a and V = U + a.
   */
  TDerived& operator+=(const T& rhs);

  /**
   * @brief V -= U and W = V -U.
   */
  TDerived& operator-=(const TDerived& rhs);

  /**
   * @brief V -= a and V = U - a.
   */
  TDerived& operator-=(const T& rhs);

  /**
   * @brief V *= a and V = U * a.
   */
  TDerived& operator*=(const T& rhs);

  /**
   * @brief V /= a and V = U / a.
   */
  TDerived& operator/=(const T& rhs);
};

/**
 * @ingroup data_classes
 * @brief Extension of `VectorArithmeticMixin` for integer values.
 * @details
 * Implement increment and decrement operators
 * in addition to `VectorArithmeticMixin` operators.
 */
template <typename T, typename TDerived>
struct IntVectorArithmeticMixin : VectorArithmeticMixin<T, TDerived>, boost::unit_steppable<TDerived> {

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
TDerived& VectorArithmeticMixin<T, TDerived>::operator+=(const TDerived& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      rhs.begin(),
      static_cast<TDerived*>(this)->begin(),
      std::plus<>());
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived>::operator+=(const T& rhs) {
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
TDerived& VectorArithmeticMixin<T, TDerived>::operator-=(const TDerived& rhs) {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      rhs.begin(),
      static_cast<TDerived*>(this)->begin(),
      std::minus<>());
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived>::operator-=(const T& rhs) {
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
TDerived& VectorArithmeticMixin<T, TDerived>::operator*=(const T& rhs) {
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
TDerived& VectorArithmeticMixin<T, TDerived>::operator/=(const T& rhs) {
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
TDerived& IntVectorArithmeticMixin<T, TDerived>::operator++() {
  std::transform(TDerived::begin(), TDerived::end(), TDerived::begin(), [](auto rhs) {
    return ++rhs;
  });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& IntVectorArithmeticMixin<T, TDerived>::operator--() {
  std::transform(TDerived::begin(), TDerived::end(), TDerived::begin(), [](auto rhs) {
    return --rhs;
  });
  return static_cast<TDerived&>(*this);
}

} // namespace Fits
} // namespace Euclid

#endif
