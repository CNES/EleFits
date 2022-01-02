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

#if defined(_ELEFITSDATA_VECTORARITHMETIC_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/VectorArithmetic.h"

namespace Euclid {
namespace Fits {

template <typename T, typename TDerived>
TDerived VectorArithmeticMixin<T, TDerived, false>::operator+() const {
  return *this; // Copies
}

template <typename T, typename TDerived>
TDerived VectorArithmeticMixin<T, TDerived, false>::operator-() const {
  TDerived res = static_cast<const TDerived&>(*this);
  std::transform(res.begin(), res.end(), res.begin(), [&](auto r) {
    return -r;
  });
  return res;
}

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
template <typename TFunc, typename... TContainers>
TDerived& VectorArithmeticMixin<T, TDerived, false>::generate(TFunc&& func, const TContainers&... args) {
  auto its = std::make_tuple(args.begin()...);
  for (auto& v : static_cast<TDerived&>(*this)) {
    v = iteratorTupleApply(its, func);
  }
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
template <typename TFunc, typename... TContainers>
TDerived& VectorArithmeticMixin<T, TDerived, false>::apply(TFunc&& func, const TContainers&... args) {
  return generate(std::forward<TFunc>(func), static_cast<TDerived&>(*this), args...);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, true>::operator++() {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [](auto rhs) {
        return ++rhs;
      });
  return static_cast<TDerived&>(*this);
}

template <typename T, typename TDerived>
TDerived& VectorArithmeticMixin<T, TDerived, true>::operator--() {
  std::transform(
      static_cast<TDerived*>(this)->begin(),
      static_cast<TDerived*>(this)->end(),
      static_cast<TDerived*>(this)->begin(),
      [](auto rhs) {
        return --rhs;
      });
  return static_cast<TDerived&>(*this);
}

} // namespace Fits
} // namespace Euclid

#endif
