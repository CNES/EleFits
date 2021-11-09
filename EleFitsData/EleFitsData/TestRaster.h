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

#ifndef _ELEFITSDATA_TESTRASTER_H
#define _ELEFITSDATA_TESTRASTER_H

#include "EleFitsData/Raster.h"
#include "EleFitsData/TestUtils.h"

#include <algorithm>
#include <complex>
#include <string>

namespace Euclid {
namespace Fits {
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
   * @brief Shortcut for rasterApprox
   */
  bool approx(const Raster<float>& other, double tol = 0.01) const;

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
template <typename T, long n = 2>
class RandomRaster : public VecRaster<T, n> {

public:
  /**
   * @brief Generate a Raster with given shape.
   */
  explicit RandomRaster(Position<n> rasterShape, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomRaster() = default;

  /**
   * @brief Shortcut for rasterApprox
   */
  bool approx(const Raster<T, n>& other, double tol = 0.01) const;
};

/**
 * @brief Check whether a test raster is approximately equal to a reference raster.
 * @details
 * Test each pixel as: |ref - test| / test < tol
 */
template <typename T, long n>
bool rasterApprox(const Raster<T, n>& test, const Raster<T, n>& ref, double tol = 0.01);

} // namespace Test
} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_TESTRASTER_IMPL
#include "EleFitsData/impl/TestRaster.hpp"
#undef _ELEFITSDATA_TESTRASTER_IMPL
/// @endcond

#endif
