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

#ifndef _EL_FITSFILE_IMAGERASTER_H
#define _EL_FITSFILE_IMAGERASTER_H

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/Subraster.h"

#include <fitsio.h>
#include <functional>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_handlers
 * @brief Reader-writer for the image data unit.
 * @tparam T The desired pixel type
 * @tparam n The desired raster dimension (or -1 for dynamic dimension)
 * @warning
 * Filling or copying a subraster is much slower than filling or copying a raster,
 * because pixels in a subraster aren't stored contiguously.
 * Use subraster-based methods with care!
 * @see Raster
 * @see Subraster
 */
template <typename T, long n = 2>
class ImageRaster {
private:
  friend class ImageHdu;

  /**
   * @brief Constructor.
   */
  ImageRaster(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @name Image properties.
   */
  /// @{

  /**
   * @brief Read the image pixel value type.
   */
  const std::type_info& readTypeid() const;

  /**
   * @brief Read the number of pixels in the image.
   */
  long readSize() const;

  /**
   * @brief Read the image shape.
   */
  Position<n> readShape() const;

  /**
   * @brief Update the image shape.
   * @details
   * Only the axis length can be changed, not the number of axes, or pixel type.
   * For more control, see `reInit()`.
   */
  void updateShape(const Position<n>& shape) const;

  /**
   * @brief Reset the image shape and pixel type.
   * @details
   * The current `ImageRaster` object is made invalid, and a new one is returned.
   */
  template <typename U, long m>
  ImageRaster<U, m> reInit(const Position<m>& shape);

  /// @}
  /**
   * @name Read the whole data unit.
   */
  /// @{

  /**
   * @brief Read the whole data unit as a new `VecRaster`.
   * @details
   * There are several options to read the whole data unit:
   * - as a new `VecRaster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Subraster` object.
   * 
   * In the last two cases, the raster or subraster is assumed to already have a conforming shape.
   * 
   * @warning
   * Filling a `Subraster` is much slower than filling a `Raster`.
   */
  VecRaster<T, n> read() const;

  /**
   * @brief Read the whole data unit into an existing `Raster`.
   * @copydetails read()
   */
  void readTo(Raster<T, n>& raster) const;

  /**
   * @brief Read the whole data unit into an existing `Subraster`.
   * @copydetails read()
   */
  void readTo(Subraster<T, n>& subraster) const;

  /// @}
  /**
   * @name Read a region of the data unit.
   */
  /// @{

  /**
   * @brief Read a region as a new `VecRaster`.
   * @details
   * There are several options to read a region of the data unit:
   * - as a new `VecRaster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Subraster` object.
   * 
   * In the last two cases, the raster or subraster is assumed to already have a conforming shape.
   */
  VecRaster<T, n> readRegion(const Region<n>& region) const;

  /**
   * @brief Read a region of the data unit into an existing `Raster`.
   * @copydetails readRegion()
   */
  void readRegionTo(const Region<n>& region, Raster<T, n>& raster) const;

  /**
   * @brief Read a region of the data unit into an existing `Subraster`.
   * @copydetails readRegion()
   */
  void readRegionTo(const Region<n>& region, Subraster<T, n>& subraster) const;

  /// @}
  /**
   * @name Write the whole data unit.
   */
  /// @{

  /**
   * @brief Write the whole data unit.
   */
  void write(const Raster<T, n>& raster) const;

  /// @}
  /**
   * @name Write a region of the data unit.
   */
  /// @{

  /**
   * @brief Write a `Raster` at a given position of the data unit.
   * @details
   * A raster or subraster can be written as a region of the data unit.
   * The given position is the front of the destination region.
   * The back of the destination region is deduced from its front and the raster or subraster shape.
   */
  void writeRegion(const Raster<T, n>& raster, const Position<n>& destination);

  /**
   * @brief Write a `Subraster` at a corresponding position of the data unit.
   * @copydetails writeRegion()
   */
  void writeRegion(const Subraster<T, n>& subraster);

  /**
   * @brief Write a `Subraster` at a given position of the data unit.
   * @copydetails writeRegion()
   */
  void writeRegion(const Subraster<T, n>& subraster, const Position<n>& destination);

  /// @}

private:
  /**
   * @brief The fitsfile.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The function to declare that the header was touched.
   */
  std::function<void(void)> m_touch;

  /**
   * @brief The function to declare that the header was edited.
   */
  std::function<void(void)> m_edit;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_IMAGERASTER_IMPL
#include "EL_FitsFile/impl/ImageRaster.hpp"
#undef _EL_FITSFILE_IMAGERASTER_IMPL
/// @endcond

#endif
