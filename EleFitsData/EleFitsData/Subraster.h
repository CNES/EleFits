// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_SUBRASTER_H
#define _ELEFITSDATA_SUBRASTER_H

#include "EleFitsData/Raster.h"
#include "EleFitsData/Region.h"

#include <algorithm> // accumulate
#include <functional> // multiplies

namespace Euclid {
namespace Fits {

/// @cond INTERNAL

/**
 * @ingroup image_data_classes
 * @brief A subraster as a view of a raster region.
 * @details
 * As opposed to a Raster, values of a Subraster are generally not contiguous in memory:
 * they are piece-wise contiguous only.
 * 
 * When a region is indeed contiguous, it is better to rely on a PtrRaster instead:
 * \code
 * VecRaster<char, 3> raster({800, 600, 3});
 * 
 * // Good :)
 * auto region = Region<3>::fromOver({100, 100, 0}, {100, 100, 3});
 * Subraster<char, 3> subraster {raster, region};
 * 
 * // Bad :(
 * auto slice = Region<3>::fromTo({0, 0, 1}, {-1, -1, 1});
 * Subraster<char, 3> contiguousSubraster {raster, slice};
 * 
 * // Good :)
 * PtrRaster<char, 2> ptrRaster({800, 600}, &raster[{0, 0, 1}]);
 * \endcode
 */
template <typename T, long N, typename THolder> // FIXME simplify as TParent
class Subraster {

public:
  using Parent = Raster<T, N, THolder>;

  /**
   * @brief Constructor.
   */
  Subraster(const Parent& parent, const Region<N>& region) : m_c_parent(&parent), m_parent(nullptr), m_region(region) {}

  /**
   * @brief Constructor.
   */
  Subraster(Parent& parent, const Region<N>& region) : m_c_parent(&parent), m_parent(&parent), m_region(region) {}

  /**
   * @brief The subraster shape.
   */
  Position<N> shape() const {
    return m_region.shape();
  }

  /**
   * @brief The number of pixels in the subraster.
   */
  long size() const {
    return m_region.size();
  }

  /**
   * @brief The parent raster.
   */
  const Parent& parent() const {
    return *m_c_parent;
  }

  /**
   * @copydoc parent()
   */
  Parent& parent() {
    return *m_parent;
  }

  /**
   * @brief The region.
   */
  const Region<N>& region() const {
    return m_region;
  }

  /**
   * @brief Pixel at given position.
   */
  const T& operator[](const Position<N>& pos) const {
    return (*m_c_parent)[pos + m_region.front];
  }

  /**
   * @brief Pixel at given position.
   */
  T& operator[](const Position<N>& pos) {
    return (*m_parent)[pos + m_region.front];
  }

private:
  /**
   * @brief Read-only pointer to the raster.
   */
  const Parent* m_c_parent;

  /**
   * @brief Read/write pointer to the raster.
   */
  Parent* m_parent;

  /**
   * @brief The region.
   */
  Region<N> m_region;
};

/// @endcond

} // namespace Fits
} // namespace Euclid

#endif
