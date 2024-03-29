// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_TESTRASTER_H
#define _ELEFITSDATA_TESTRASTER_H

#include "EleFitsData/Raster.h" // ELEFITS_FOREACH_RASTER_TYPE
#include "EleFitsData/TestUtils.h"
#include "Linx/Data/Raster.h"

#include <algorithm>
#include <complex>
#include <string>

namespace Fits {
namespace Test {

/**
 * @brief A 2D image Raster of floats.
 */
class SmallRaster : public Linx::Raster<float> { // FIXME useless with Linx::random()
public:

  /**
   * @brief Generate a SmallRaster with given width and height.
   */
  SmallRaster(Linx::Index width = 3, Linx::Index height = 2);

  /** @brief Destructor. */
  virtual ~SmallRaster() = default;

  /**
   * @brief Shortcut for raster_approx
   */
  template <typename TRaster>
  bool approx(const TRaster& other, double tol = 0.01) const;

  /**
   * @brief Raster width.
   */
  Linx::Index width;

  /**
   * @brief Raster height.
   */
  Linx::Index height;
};

/**
 * @brief A random Raster of given type and shape.
 */
template <typename T, Linx::Index N = 2>
class RandomRaster : public Linx::Raster<T, N> {
public:

  /**
   * @brief Generate a Raster with given shape.
   */
  explicit RandomRaster(Linx::Position<N> raster_shape, T min = almost_min<T>(), T max = almost_max<T>());

  /**
   * @brief Destructor.
   */
  virtual ~RandomRaster() = default;

  /**
   * @brief Shortcut for raster_approx
   */
  template <typename TRaster>
  bool approx(const TRaster& other, double tol = 0.01) const;
};

/**
 * @brief Check whether a test raster is approximately equal to a reference raster.
 * @details
 * Test each pixel as: |ref - test| / test < tol
 */
template <typename TRaster, typename URaster>
bool raster_approx(const TRaster& test, const URaster& ref, double tol = 0.01);

} // namespace Test
} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_TESTRASTER_IMPL
#include "EleFitsData/impl/TestRaster.hpp"
#undef _ELEFITSDATA_TESTRASTER_IMPL
/// @endcond

#endif
