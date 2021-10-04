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
#include "EL_FitsFile/MemFileRegions.h"

#include <fitsio.h>
#include <functional>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_handlers
 * @brief Reader-writer for the image data unit.
 * @tparam T The desired pixel type
 * @tparam n The desired raster dimension (or -1 for dynamic dimension)
 * @details
 * This handler provides methods to access image metadata (image-related keyword records) and data.
 * 
 * Data can be read and written region-wise.
 * To specify the source and destination regions, three positions are required:
 * - a source position,
 * - a destination position,
 * - a shape.
 * 
 * The position in the file is given as a method parameter.
 * The destination position is 0 for rasters, or the front position of a subraster.
 * The shape is either in a region parameter or is the size of a raster or subraster.
 * @warning
 * Filling or copying a subraster is much slower than filling or copying a raster,
 * because pixels in a subraster aren't stored contiguously.
 * Use subraster-based methods with care!
 * @see Raster
 * @see Subraster
 */
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
  template <long n = 2>
  Position<n> readShape() const;

  /**
   * @brief Update the image shape.
   */
  template <long n = 2>
  void updateShape(const Position<n>& shape) const;

  /**
   * @brief Update the image type and shape.
   */
  template <typename T, long n = 2>
  void reinit(const Position<n>& shape) const;

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
  template <typename T, long n = 2>
  VecRaster<T, n> read() const;

  /**
   * @brief Read the whole data unit into an existing `Raster`.
   * @copydetails read()
   */
  template <typename T, long n = 2>
  void readTo(Raster<T, n>& raster) const;

  /**
   * @brief Read the whole data unit into an existing `Subraster`.
   * @copydetails read()
   */
  template <typename T, long n = 2>
  void readTo(Subraster<T, n>& subraster) const;

  /// @}
  /**
   * @name Read a region of the data unit.
   */
  /// @{

  /**
   * @brief Read a region as a new `VecRaster`.
   * @param region The HDU region to be read
   * @param regions The in-memory and in-file regions
   * @param frontPosition The front position of the HDU region to be read
   * @param raster The destination raster
   * @param subraster The destination subraster
   * @details
   * There are several options to read a region of the data unit:
   * - as a new `VecRaster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Subraster` object.
   * In the last two cases, the in-file and in-memory regions are given as a `MemFileRegions` object.
   * 
   * For example, to read the HDU region from position (50, 80) to position (100, 120)
   * into an existing raster at position (25, 40), do:
   * \code
   * const MemFileRegions<2> regions({25, 40}, {{50, 80}, {100, 120}});
   * image.readRegionTo(regions, raster);
   * \endcode
   * where `image` is the `ImageRaster` and `raster` is the `Raster`.
   * 
   * In simpler cases, where the in-file or in-memory front position is 0,
   * factories can be used, e.g. to read into position 0:
   * \code
   * image.readRegionTo(makeFileRegion({50, 80}, {100, 120}), raster);
   * \endcode
   */
  template <typename T, long n = 2>
  VecRaster<T, n> readRegion(const Region<n>& region) const;

  /**
   * @brief Read a region of the data unit into a region of an existing `Raster`.
   * @copydetails readRegion()
   */
  template <typename T, long m = 2, long n = 2>
  void readRegionTo(const MemFileRegions<m>& regions, Raster<T, n>& raster) const;

  /**
   * @brief Read a region of the data unit into an existing `Raster`.
   * @copydetails readRegion()
   */
  template <typename T, long n = 2>
  void readRegionTo(const Position<n>& frontPosition, Raster<T, n>& raster) const;

  /**
   * @brief Read a region of the data unit into an existing `Subraster`.
   * @copydetails readRegion()
   */
  template <typename T, long n = 2>
  void readRegionTo(const Position<n>& frontPosition, Subraster<T, n>& subraster) const;

  /// @}
  /**
   * @name Write the whole data unit.
   */
  /// @{

  /**
   * @brief Write the whole data unit.
   */
  template <typename T, long n = 2>
  void write(const Raster<T, n>& raster) const;

  /// @}
  /**
   * @name Write a region of the data unit.
   */
  /// @{

  /**
   * @brief Write a `Raster` at a given position of the data unit.
   * @param frontPosition The front position of the HDU region to be written
   * @param raster The raster to be written
   * @param subraster The subraster to be written
   * @details
   * A raster or subraster can be written as a region of the data unit.
   * The given position is the front of the destination region.
   * The back of the destination region is deduced from its front and the raster or subraster shape.
   */
  template <typename T, long n = 2>
  void writeRegion(const Position<n>& frontPosition, const Raster<T, n>& raster) const;

  /**
   * @brief Write a `Subraster` at a corresponding position of the data unit.
   * @copydetails writeRegion()
   */
  template <typename T, long n = 2>
  void writeRegion(const Subraster<T, n>& subraster) const;

  /**
   * @brief Write a `Subraster` at a given position of the data unit.
   * @copydetails writeRegion()
   */
  template <typename T, long n = 2>
  void writeRegion(const Position<n>& frontPosition, const Subraster<T, n>& subraster) const;

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
