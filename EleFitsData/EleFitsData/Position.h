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
namespace Fits {

/**
 * @ingroup image_data_classes
 * @brief _n_-dimensional pixel position or image shape, i.e. set of integer coordinates.
 * @tparam N A non-negative dimension (0 is allowed), or -1 for variable dimension.
 * @details
 * The values are stored in a `std::array<long, N>` in general (`N >= 0`),
 * or `std::vector<long>` for variable dimension (`N = -1`).
 *
 * Memory and services are optimized when dimension is fixed at compile-time (`N >= 0`).
 * 
 * Anonymous brace-initialization is permitted, e.g.:
 * \code
 * VecRaster<float> raster({1920, 1080});
 * // Is equivalent to
 * VecRaster<float> raster(Position<2>({1920, 1080}));
 * \endcode
 * 
 * Classical positions are instantiated with named constructors, e.g.:
 * \code
 * auto bottomLeft = Position<2>::zero();
 * auto topRight = Position<2>::max();
 * \endcode
 * 
 * @see Region
 */
template <long N = 2>
struct Position {

  static constexpr long Dim = N;

  /**
   * @brief Storage class for the indices.
   */
  using Indices = typename std::conditional<(N == -1), std::vector<long>, std::array<long, (std::size_t)N>>::type;

  /**
   * @brief Standad-like alias to the value type for compatibility, e.g. with Boost.
   */
  using value_type = long;

  /**
   * @brief Standad-like alias to the iterator type for compatibility, e.g. with Boost.
   */
  using iterator = typename Indices::iterator;

  /**
   * @brief Standad-like alias to the const iterator type for compatibility, e.g. with Boost.
   */
  using const_iterator = typename Indices::const_iterator;

  /**
   * @brief Default constructor.
   * @warning
   * The indices are unspecified.
   * To create position 0, use `zero()` instead.
   */
  Position();

  /**
   * @brief Create a position of given dimension.
   */
  explicit Position(long dim); // FIXME weird! Replace with single CTor Position(Ts... args)?

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
  static Position<N> zero() {
    Position<N> res; // FIXME valid for N = -1? Position<N> res(std::abs(N))?
    for (auto& i : res) {
      i = 0;
    }
    return res;
  }

  /**
   * @brief Create max position (full of -1's).
   */
  static Position<N> max() {
    Position<N> res; // FIXME valid for N = -1? Position<N> res(std::abs(N))?
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
  iterator begin() {
    return indices.begin();
  }

  /**
   * @copydoc begin()
   */
  const_iterator begin() const {
    return indices.begin();
  }

  /**
   * @brief Iterator to the past-the-last element.
   */
  iterator end() {
    return indices.end();
  }

  /**
   * @copydoc end()
   */
  const_iterator end() const {
    return indices.end();
  }

  /**
   * @brief Create a position of lower dimension.
   * @tparam M The new dimension; cannot be -1
   * @details
   * The indices up to dimension `M` are copied.
   */
  template <long M>
  Position<M> slice() const {
    Position<M> res; // TODO one-line with iterator?
    for (long i = 0; i < M; ++i) {
      res[i] = indices[i];
    }
    return res;
  }

  /**
   * @brief Create a position of higher dimension.
   * @tparam M The new dimension; cannot be -1
   * @details
   * The indices up to dimension `N` are copied.
   * Those between dimensions `N` and `M` are taken from the given position.
   */
  template <long M>
  Position<M> extend(const Position<M>& padding) const {
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
template <long N = 2>
long shapeSize(const Position<N>& shape) {
  return std::accumulate(shape.begin(), shape.end(), 1L, std::multiplies<long>());
}

template <>
Position<-1>::Position(long dim);

template <>
Position<-1>::Position(std::initializer_list<long> posIndices);

template <>
template <typename TIterator>
Position<-1>::Position(TIterator begin, TIterator end);

template <long N>
Position<N>::Position() : indices {} {
  for (auto& i : indices) {
    i = 0; // FIXME not needed: indices are unspecified; but should be tested for non-regression
  }
}

template <long N>
Position<N>::Position(long dim) : indices {} {
  if (dim != N) {
    throw FitsError("Dimension mismatch."); // TODO clarify
  }
}

template <long N>
Position<N>::Position(std::initializer_list<long> posIndices) : Position<N> {posIndices.begin(), posIndices.end()} {}

template <long N>
template <typename TIterator>
Position<N>::Position(TIterator begin, TIterator end) : indices {} {
  std::copy(begin, end, indices.begin());
}

template <>
template <typename TIterator>
Position<-1>::Position(TIterator begin, TIterator end) : indices(begin, end) {}

template <long N = 2>
bool operator==(const Position<N>& lhs, const Position<N>& rhs) {
  return lhs.indices == rhs.indices;
}

template <long N = 2>
bool operator!=(const Position<N>& lhs, const Position<N>& rhs) {
  return lhs.indices != rhs.indices;
}

/**
 * @brief Add a position.
 */
template <long N = 2>
Position<N>& operator+=(Position<N>& lhs, const Position<N>& rhs) {
  std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::plus<long>());
  return lhs;
}

/**
 * @brief Subtract a position.
 */
template <long N = 2>
Position<N>& operator-=(Position<N>& lhs, const Position<N>& rhs) {
  std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), std::minus<long>());
  return lhs;
}

/**
 * @brief Add a scalar to each coordinate.
 */
template <long N = 2>
Position<N>& operator+=(Position<N>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i + rhs;
  });
  return lhs;
}

/**
 * @brief Subtract a scalar to each coordinate.
 */
template <long N = 2>
Position<N>& operator-=(Position<N>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i - rhs;
  });
  return lhs;
}

/**
 * @brief Multiply each coordinate by a scalar.
 */
template <long N = 2>
Position<N>& operator*=(Position<N>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i * rhs;
  });
  return lhs;
}

/**
 * @brief Divide each coordinate by a scalar.
 */
template <long N = 2>
Position<N>& operator/=(Position<N>& lhs, long rhs) {
  std::transform(lhs.begin(), lhs.end(), lhs.begin(), [=](long i) {
    return i / rhs;
  });
  return lhs;
}

/**
 * @brief Add 1 to each coordinate.
 */
template <long N = 2>
Position<N>& operator++(Position<N>& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @brief Subtract 1 to each coordinate.
 */
template <long N = 2>
Position<N>& operator--(Position<N>& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @brief Return the current position and then add 1 to each coordinate.
 */
template <long N = 2>
Position<N> operator++(Position<N>& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @brief Return the current position and then subtract 1 to each coordinate.
 */
template <long N = 2>
Position<N> operator--(Position<N>& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @brief Identity.
 */
template <long N = 2>
Position<N> operator+(const Position<N>& lhs) {
  return lhs;
}

/**
 * @brief Change the sign of each coordinate.
 */
template <long N = 2>
Position<N> operator-(const Position<N>& lhs) {
  auto res = lhs;
  std::transform(res.begin(), res.end(), res.begin(), [=](long i) {
    return -i;
  });
  return res;
}

/**
 * @brief Add two positions.
 */
template <long N = 2>
Position<N> operator+(const Position<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract two positions.
 */
template <long N = 2>
Position<N> operator-(const Position<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @brief Add a position and a scalar.
 */
template <long N = 2>
Position<N> operator+(const Position<N>& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @brief Subtract a position and a scalar.
 */
template <long N = 2>
Position<N> operator-(const Position<N>& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @brief Multiply a position by a scalar.
 */
template <long N = 2>
Position<N> operator*(const Position<N>& lhs, long rhs) {
  auto res = lhs;
  res *= rhs;
  return res;
}

/**
 * @brief Divide a position by a scalar.
 */
template <long N = 2>
Position<N> operator/(const Position<N>& lhs, long rhs) {
  auto res = lhs;
  res /= rhs;
  return res;
}

} // namespace Fits
} // namespace Euclid

#endif
