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

#ifdef _EL_FITSDATA_RASTER_IMPL

#include <functional> // multiplies
#include <numeric> // accumulate

#include "EL_FitsData/Raster.h"

namespace Euclid {
namespace FitsIO {

/// @cond INTERNAL
namespace Internal {

/**
 * @brief nD-index recursive implementation.
 * @tparam n The raster dimension.
 * @tparam i The dimension of the current recursion step, should be initialized with `n - 1`.
 */
template <long n, long i = n - 1>
struct IndexRecursionImpl {

  /**
   * @brief Index of given position in given shape for Raster::index.
   */
  static long index(const Position<n> &shape, const Position<n> &pos) {
    return std::get<n - 1 - i>(pos) + std::get<n - 1 - i>(shape) * IndexRecursionImpl<n, i - 1>::index(shape, pos);
  }
};

/**
 * @brief Terminal case: dimension 0.
 */
template <long n>
struct IndexRecursionImpl<n, 0> {
  static long index(const Position<n> &shape, const Position<n> &pos) {
    (void)(shape);
    return std::get<n - 1>(pos);
  }
};

/**
 * @brief Variable dimension case.
 */
template <long i>
struct IndexRecursionImpl<-1, i> {
  static long index(const Position<-1> &shape, const Position<-1> &pos) {
    const auto n = shape.size();
    if (pos.size() != n) {
      throw std::runtime_error(
          "Dimension mismatch. Raster is of dimension " + std::to_string(n) + " while position is of dimension " +
          std::to_string(pos.size()));
    }
    long res = 0;
    for (std::size_t j = 0; j < shape.size(); ++j) {
      res = pos[n - 1 - j] + shape[n - 1 - j] * res;
    }
    return res;
  }
};

} // namespace Internal
/// @endcond

template <typename T, long n>
Raster<T, n>::Raster(Position<n> shape_) : shape(shape_) {
}

template <typename T, long n>
inline long Raster<T, n>::dimension() const {
  return shape.size();
}

template <typename T, long n>
template <long i>
inline long Raster<T, n>::length() const {
  return std::get<i>(shape);
}

template <typename T, long n>
inline long Raster<T, n>::size() const {
  return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<long>());
}

template <typename T, long n>
inline long Raster<T, n>::index(const Position<n> &pos) const {
  return Internal::IndexRecursionImpl<n>::index(shape, pos);
}

template <typename T, long n>
inline const T &Raster<T, n>::operator[](const Position<n> &pos) const {
  return data()[index(pos)];
}

template <typename T, long n>
inline T &Raster<T, n>::operator[](const Position<n> &pos) {
  return const_cast<T &>(const_cast<const Raster *>(this)->operator[](pos));
}

template <typename T, long n>
PtrRaster<T, n>::PtrRaster(Position<n> shape_, const T *data) : Raster<T, n>(shape_), m_data(data) {
}

template <typename T, long n>
const T *PtrRaster<T, n>::data() const {
  return m_data;
}

template <typename T, long n>
VecRefRaster<T, n>::VecRefRaster(Position<n> shape_, const std::vector<T> &vecRef) :
    Raster<T, n>(shape_),
    m_ref(vecRef) {
}

template <typename T, long n>
const T *VecRefRaster<T, n>::data() const {
  return m_ref.data();
}

template <typename T, long n>
const std::vector<T> &VecRefRaster<T, n>::vector() const {
  return m_ref;
}

template <typename T, long n>
VecRaster<T, n>::VecRaster(Position<n> shape_, std::vector<T> vec) : Raster<T, n>(shape_), m_vec(vec) {
}

template <typename T, long n>
VecRaster<T, n>::VecRaster(Position<n> shape_) : Raster<T, n>(shape_), m_vec(this->size()) {
}

template <typename T, long n>
const T *VecRaster<T, n>::data() const {
  return m_vec.data();
}

template <typename T, long n>
T *VecRaster<T, n>::data() {
  return const_cast<T *>(const_cast<const VecRaster *>(this)->data());
}

template <typename T, long n>
const std::vector<T> &VecRaster<T, n>::vector() const {
  return m_vec;
}

template <typename T, long n>
std::vector<T> &VecRaster<T, n>::vector() {
  return m_vec;
}

} // namespace FitsIO
} // namespace Euclid

#endif
