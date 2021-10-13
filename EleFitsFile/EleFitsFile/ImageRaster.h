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

#include "EleFitsData/Raster.h"
#include "EleFitsFile/FileMemRegions.h"

#include <fitsio.h>
#include <functional>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup image_handlers
 * @brief Reader-writer for the image data unit.
 * @details
 * This handler provides methods to access image metadata (image-related keyword records) and data.
 * 
 * Reading methods either return a `VecRaster` or fill an existing `Raster`.
 * 
 * Data can be read and written region-wise.
 * Source and destination regions are specified by a `FileMemRegions` object.
 * 
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
   * @tparam T The desired raster type
   * @tparam m The desired raster dimension, which can be smaller than the data dimension in file
   * @tparam n The region dimension, which corresponds to the data dimension in file
   * @param region The in-file region
   * @param regions The in-memory and in-file regions
   * @param frontPosition The in-file region front position
   * @param raster The destination raster
   * @param subraster The destination subraster
   * @details
   * There are several options to read a region of the data unit:
   * - as a new `VecRaster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Subraster` object.
   * In the last two cases, the in-file and in-memory regions are given as a `FileMemRegions` object.
   * 
   * For example, to read the HDU region from position (50, 80) to position (100, 120)
   * into an existing raster at position (25, 40), do:
   * \code
   * const FileMemRegions<2> regions({ 25, 40 }, { { 50, 80 }, { 100, 120 } });
   * image.readRegionTo(regions, raster);
   * \endcode
   * where `image` is the `ImageRaster` and `raster` is the `Raster`.
   * 
   * In simpler cases, where the in-file or in-memory front position is 0,
   * factories can be used, e.g. to read into position 0 of the raster:
   * \code
   * image.readRegionTo<2>({ { 50, 80 }, { 100, 120 } }, raster);
   * \endcode
   */
  template <typename T, long m, long n>
  VecRaster<T, m> readRegion(const Region<n>& region) const;

  /**
   * @brief Read a region of the data unit into a region of an existing `Raster`.
   * @copydetails readRegion()
   */
  template <typename T, long m, long n>
  void readRegionTo(FileMemRegions<n> regions, Raster<T, m>& raster) const;

  /// @}
  /**
   * @name Write the whole data unit.
   */
  /// @{

  /**
   * @brief Write the whole data unit.
   */
  template <typename T, long n>
  void write(const Raster<T, n>& raster) const;

  /// @}
  /**
   * @name Write a region of the data unit.
   */
  /// @{

  /**
   * @brief Write a `Raster` at a given position of the data unit.
   * @param regions The in-file and in-memory regions
   * @param raster The raster to be written
   * @details
   * In-file and in-memory (raster) regions are specified as the first parameter.
   * Max bounds (-1) can be used in one, several, or all axes.
   * Shortcuts offered by `FileMemRegions` and `Region` can be used to implement special cases:
   * \code
   * // Write the whole raster at position (10, 20, 30)
   * du.writeRegion<3>({ 10, 20, 30 }, raster);
   * 
   * // Write the whole HDU with a region of the raster starting at (10, 20, 30)
   * du.writeRegion<3>({ Region<3>::whole(), { 10, 20, 30 } }, raster);
   * \endcode
   * 
   * Note that the raster dimension can be lower than the HDU dimension.
   * For example, it is possible to write a 2D raster in a 3D HDU.
   * \code
   * // Write the 3rd plane of raster into the 5th plane of the HDU
   * du.writeRegion<3>({ { 0, 0, 4 } }, raster.section(2));
   * \endcode
   */
  template <typename T, long m, long n>
  void writeRegion(FileMemRegions<n> regions, const Raster<T, m>& raster) const; // FIXME return bool = isContiguous()?

  /**
   * @brief Write a `Subraster` at a corresponding position of the data unit.
   * @copydetails writeRegion()
   */
  template <typename T, long n>
  void writeRegion(const Subraster<T, n>& subraster) const;

private:
  /**
   * @brief Read a region of the data unit into an existing `Raster`.
   * @copydetails readRegion()
   */
  template <typename T, long m, long n>
  void readRegionToSlice(const Position<n>& frontPosition, Raster<T, m>& raster) const;

  /**
   * @brief Read a region of the data unit into an existing `Subraster`.
   * @copydetails readRegion()
   */
  template <typename T, long m, long n>
  void readRegionToSubraster(const Position<n>& frontPosition, Subraster<T, m>& subraster) const;

  /**
   * @brief Read a region of the data unit into an existing `Subraster`.
   * @details
   * The region is that of the subraster.
   * This function is equivalent to:
   * \code
   * readRegionTo({region, region}, raster);
   * \endcode
   * where `region` would be the subraster region, and `raster` the subraster parent.
   */
  template <typename T, long n>
  void readRegionTo(Subraster<T, n>& subraster) const;

  /**
   * @brief Write a `Raster` at a given position of the data unit.
   */
  template <typename T, long m, long n>
  void writeSlice(const Position<n>& frontPosition, const Raster<T, m>& raster) const;

  /**
   * @brief Write a `Subraster` at a given position of the data unit.
   */
  template <typename T, long m, long n>
  void writeSubraster(const Position<n>& frontPosition, const Subraster<T, m>& subraster) const;

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
#include "EleFitsFile/impl/ImageRaster.hpp"
#undef _EL_FITSFILE_IMAGERASTER_IMPL
/// @endcond

#endif
