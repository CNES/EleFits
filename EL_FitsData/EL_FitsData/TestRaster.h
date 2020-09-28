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

#ifndef _EL_FITSDATA_TESTRASTER_H
#define _EL_FITSDATA_TESTRASTER_H

#include <algorithm>
#include <complex>
#include <string>

#include "EL_FitsData/TestUtils.h"
#include "EL_FitsData/Raster.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief A 2D image Raster of floats.
 */
class SmallRaster : public VecRaster<float> {

public:
  /**
   * @brief Generate a SmallRaster with given width and height.
   */
  SmallRaster(long width = 3, long height = 2);

  /** @brief Destructor. */
  virtual ~SmallRaster() = default;

  /**
   * @brief Check whether the Raster is approximately equal to another Raster.
   * @details
   * Test each pixel as: (other - this) / this < tol
   */
  bool approx(const Raster<float> &other, float tol = 0.1F) const;

  /**
   * @brief Raster width.
   */
  long width;

  /**
   * @brief Raster height.
   */
  long height;
};

/**
 * @brief A random Raster of given type and shape.
 */
template <typename T, long n>
class RandomRaster : public VecRaster<T, n> {

public:
  /**
   * @brief Generate a Raster with given shape.
   */
  explicit RandomRaster(Position<n> rasterShape, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomRaster() = default;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

#define _EL_FITSDATA_TESTRASTER_IMPL
#include "EL_FitsData/impl/TestRaster.hpp"
#undef _EL_FITSDATA_TESTRASTER_IMPL

#endif
