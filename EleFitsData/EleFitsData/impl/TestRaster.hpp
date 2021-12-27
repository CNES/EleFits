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

#if defined(_ELEFITSDATA_TESTRASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/TestRaster.h"
#include "EleFitsData/TestUtils.h"

namespace Euclid {
namespace Fits {
namespace Test {

template <typename TContainer>
bool SmallRaster::approx(const Raster<float, 2, TContainer>& other, double tol) const {
  return rasterApprox(*this, other, tol);
}

template <typename T, long N>
RandomRaster<T, N>::RandomRaster(Position<N> rasterShape, T min, T max) :
    VecRaster<T, N>(rasterShape, generateRandomVector<T>(shapeSize(rasterShape), min, max)) {}

template <typename T, long N>
template <typename TContainer>
bool RandomRaster<T, N>::approx(const Raster<T, N, TContainer>& other, double tol) const {
  return rasterApprox(*this, other, tol);
}

template <typename T, long N, typename TContainer>
bool rasterApprox(const Raster<T, N, TContainer>& test, const Raster<T, N, TContainer>& ref, double tol) {
  if (test.shape() != ref.shape()) {
    return false;
  }
  for (long i = 0; i < test.size(); ++i) {
    if (not approx(test.data()[i], ref.data()[i], tol)) {
      return false;
    }
  }
  return true;
}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
