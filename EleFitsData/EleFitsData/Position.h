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

#include "EleFitsData/DataContainer.h"
#include "EleFitsData/DataUtils.h"
#include "EleFitsData/FitsError.h"

#include <numeric> // accumulate
#include <type_traits> // conditional

namespace Euclid {
namespace Fits {

template <long N = 2>
using Indices = typename std::conditional<(N == -1), std::vector<long>, std::array<long, (std::size_t)N>>::type;

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
class Position : public DataContainer<long, Indices<N>, Position<N>> {
public:
  /**
   * @brief The dimension template parameter.
   */
  static constexpr long Dim = N;

  ELEFITS_VIRTUAL_DTOR(Position)
  ELEFITS_COPYABLE(Position)
  ELEFITS_MOVABLE(Position)

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
  explicit Position(long dim);

  /**
   * @brief Create a position by copying data from some container.
   */
  template <typename TIterator>
  Position(TIterator begin, TIterator end);

  /**
   * @brief Create a position from a brace-enclosed list of indices.
   */
  Position(std::initializer_list<long> indices);

  /**
   * @brief Create position 0.
   */
  static Position<N> zero() {
    Position<N> res(std::abs(Dim));
    std::fill(res.begin(), res.end(), 0);
    return res;
  }

  /**
   * @brief Create max position (full of -1's).
   */
  static Position<N> max() {
    Position<N> res(std::abs(N));
    std::fill(res.begin(), res.end(), -1);
    return res;
  }

  /**
   * @brief Check whether the position is zero.
   */
  bool isZero() const {
    for (auto i : *this) {
      if (i != 0) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Check whether the position is max.
   */
  bool isMax() const {
    for (auto i : *this) {
      if (i != -1) {
        return false;
      }
    }
    return true;
  }

  /**
   * @brief Create a position of lower dimension.
   * @tparam M The new dimension; cannot be -1
   * @details
   * The indices up to dimension `M` are copied.
   */
  template <long M>
  Position<M> slice() const {
    const auto b = this->begin();
    auto e = b;
    std::advance(e, M);
    return Position<M>(b, e);
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
    for (long i = 0; i < this->size(); ++i) { // TODO std::transform
      res[i] = (*this)[i];
    }
    return res;
  }
};

/**
 * @brief Compute the number of pixels in a given shape.
 */
template <long N = 2>
long shapeSize(const Position<N>& shape) {
  return std::accumulate(shape.begin(), shape.end(), 1L, std::multiplies<long>());
}

template <long N>
Position<N>::Position() : DataContainer<long, Indices<N>, Position<N>>() {}

template <long N>
Position<N>::Position(long dim) : DataContainer<long, Indices<N>, Position<N>>(dim) {}

template <long N>
Position<N>::Position(std::initializer_list<long> indices) : DataContainer<long, Indices<N>, Position<N>>(indices) {}

template <long N>
template <typename TIterator>
Position<N>::Position(TIterator begin, TIterator end) : DataContainer<long, Indices<N>, Position<N>>(begin, end) {}

} // namespace Fits
} // namespace Euclid

#endif
