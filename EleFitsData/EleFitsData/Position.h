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

#ifndef _ELEFITSDATA_POSITION_H
#define _ELEFITSDATA_POSITION_H

#include "EleFitsData/FitsError.h"

#include <algorithm> // transform
#include <array>
#include <functional> // plus, minus, multiplies
#include <initializer_list>
#include <numeric> // accumulate
#include <type_traits> // conditional
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_data_classes
 * @brief _n_-dimensional pixel position or image shape, i.e. set of integer coordinates.
 * @tparam n A non-negative dimension (0 is allowed), or -1 for variable dimension.
 * @details
 * The values are stored in a `std::array<long, n>` in general (`n >= 0`),
 * or `std::vector<long>` for variable dimension (`n = -1`).
 *
 * Memory and services are optimized when dimension is fixed at compile-time (`n >= 0`).
 */
template <long n = 2>
struct Position {

  static constexpr long Dim = n;

  /**
   * @brief Storage class for the indices.
   */
  using Indices = typename std::conditional<(n == -1), std::vector<long>, std::array<long, (std::size_t)n>>::type;

  using Iterator = typename Indices::iterator;

  using ConstIterator = typename Indices::const_iterator;

  /**
   * @brief Create position 0.
   * @deprecated Use `zero()` instead for better clarity.
   */
  Position();

  /**
   * @brief Create a position of given dimension.
   */
  explicit Position(long dim); // FIXME weird!

  /**
   * @brief Create a position from a brace-enclosed list of indices.
   */
  Position(std::initializer_list<long> posIndices);

  /**
   * @brief Create a position by copying data from some container.
   */
  template <typename TIterator>
  Position(TIterator begin, TIterator end);

  /**
   * @brief Create position 0.
   */
  static Position<n> zero() {
    return {};
  }

  /**
   * @brief Create max position (full of -1's).
   */
  static Position<n> max() {
    Position<n> res; // FIXME valid for n = -1?
    for (auto& i : res) {
      i = -1;
    }
    return res;
  }

  /**
   * @brief Check whether the position is zero.
   */
  bool isZero() const {
    return *this == zero();
  }

  /**
   * @brief Check whether the position is max.
   */
  bool isMax() const {
    return *this == max();
  }

  /**
   * @brief The number of indices.
   */
  long size() const {
    return indices.size();
  }

  /**
   * @brief Access the underlying array.
   */
  const long* data() const {
    return indices.data();
  }

  /**
   * @copydoc data()
   */
  long* data() {
    return indices.data();
  }

  /**
   * @brief Access the `i`-th element.
   */
  long& operator[](long i) {
    return indices[i];
  }

  /**
   * @copydoc operator[]()
   */
  const long& operator[](long i) const {
    return indices[i];
  }

  /**
   * @brief Iterator to the first element.
   */
  Iterator begin() {
    return indices.begin();
  }

  /**
   * @copydoc begin()
   */
  ConstIterator begin() const {
    return indices.begin();
  }

  /**
   * @brief Iterator to the past-the-last element.
   */
  Iterator end() {
    return indices.end();
  }

  /**
   * @copydoc end()
   */
  ConstIterator end() const {
    return indices.end();
  }

  /**
   * @brief Create a position of lower dimension.
   * @tparam m The new dimension; cannot be -1
   * @details
   * The indices up to dimension `m` are copied.
   */
  template <long m>
  Position<m> slice() const {
    Position<m> res; // TODO one-line with iterator?
    for (long i = 0; i < m; ++i) {
      res[i] = indices[i];
    }
    return res;
  }

  /**
   * @brief Create a position of higher dimension.
   * @tparam m The new dimension; cannot be -1
   * @details
   * The indices up to dimension `n` are copied.
   * Those between dimensions `n` and `m` are taken from the given position.
   */
  template <long m>
  Position<m> extend(const Position<m>& padding) const {
    auto res = padding;
    for (long i = 0; i < size(); ++i) { // TODO std::transform
      res[i] = indices[i];
    }
    return res;
  }

  /**
   * @brief The indices.
   */
  Indices indices;
};

/**
 * @brief Compute the number of pixels in a given shape.
 */
template <long n = 2>
long shapeSize(const Position<n>& shape) {
  return std::accumulate(shape.begin(), shape.end(), 1L, std::multiplies<long>());
}

template <>
Position<-1>::Position(long dim);

template <>
Position<-1>::Position(std::initializer_list<long> posIndices);

template <>
template <typename TIterator>
Position<-1>::Position(TIterator begin, TIterator end);

template <long n>
Position<n>::Position() : indices {} {
  for (auto& i : indices) {
    i = 0;
  }
}

template <long n>
Position<n>::Position(long dim) : indices {} {
  if (dim != n) {
    throw FitsError("Dimension mismatch."); // TODO clarify
  }
}

template <long n>
Position<n>::Position(std::initializer_list<long> posIndices) : Position<n> { posIndices.begin(), posIndices.end() } {}

template <long n>
template <typename TIterator>
Position<n>::Position(TIterator begin, TIterator end) : indices {} {
  std::copy(begin, end, indices.begin());
}

template <>
template <typename TIterator>
Position<-1>::Position(TIterator begin, TIterator end) : indices(begin, end) {}

template <long n = 2>
bool operator==(const Position<n>& lhs, const Position<n>& rhs) {
  return lhs.indices == rhs.indices;
}

template <long n = 2>
bool operator!=(const Position<n>& lhs, const Position<n>& rhs) {
  return lhs.indices != rhs.indices;
}

/**
 * @brief Add a position.
 */
template <long n = 2>
Position<n>& operator+=(Position<n>& lhs, const Position<n>& rhs) {
  std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::plus<long>());
  return lhs;
}

/**
 * @brief Subtract a position.
 */
template <long n = 2>
Position<n>& operator-=(Position<n>& lhs, const Position<n>& rhs) {
  std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::minus<long>());
  return lhs;
}

/**
 * @brief Add a scalar to each coordinate.
 */
template <long n = 2>
Position<n>& operator+=(Position<n>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i + rhs;
  });
  return lhs;
}

/**
 * @brief Subtract a scalar to each coordinate.
 */
template <long n = 2>
Position<n>& operator-=(Position<n>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i - rhs;
  });
  return lhs;
}

/**
 * @brief Multiply each coordinate by a scalar.
 */
template <long n = 2>
Position<n>& operator*=(Position<n>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i * rhs;
  });
  return lhs;
}

/**
 * @brief Divide each coordinate by a scalar.
 */
template <long n = 2>
Position<n>& operator/=(Position<n>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i / rhs;
  });
  return lhs;
}

/**
 * @brief Add 1 to each coordinate.
 */
template <long n = 2>
Position<n>& operator++(Position<n>& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @brief Subtract 1 to each coordinate.
 */
template <long n = 2>
Position<n>& operator--(Position<n>& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @brief Return the current position and then add 1 to each coordinate.
 */
template <long n = 2>
Position<n> operator++(Position<n>& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @brief Return the current position and then subtract 1 to each coordinate.
 */
template <long n = 2>
Position<n> operator--(Position<n>& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @brief Identity.
 */
template <long n = 2>
Position<n> operator+(const Position<n>& lhs) {
  return lhs;
}

/**
 * @brief Change the sign of each coordinate.
 */
template <long n = 2>
Position<n> operator-(const Position<n>& lhs) {
  auto res = lhs;
  std::transform(res.begin(), res.end(), res.begin(), [=](long i) {
    return -i;
  });
  return res;
}

/**
 * @brief Add two positions.
 */
template <long n = 2>
Position<n> operator+(const Position<n>& lhs, const Position<n>& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract two positions.
 */
template <long n = 2>
Position<n> operator-(const Position<n>& lhs, const Position<n>& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @brief Add a position and a scalar.
 */
template <long n = 2>
Position<n> operator+(const Position<n>& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract a position and a scalar.
 */
template <long n = 2>
Position<n> operator-(const Position<n>& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @brief Multiply a position by a scalar.
 */
template <long n = 2>
Position<n> operator*(const Position<n>& lhs, long rhs) {
  auto res = lhs;
  res *= rhs;
  return res;
}

/**
 * @brief Divide a position by a scalar.
 */
template <long n = 2>
Position<n> operator/(const Position<n>& lhs, long rhs) {
  auto res = lhs;
  res /= rhs;
  return res;
}

} // namespace FitsIO
} // namespace Euclid

#endif
