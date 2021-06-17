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

#ifndef _EL_FITSFILE_IMAGEHDU_H
#define _EL_FITSFILE_IMAGEHDU_H

#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_FitsData/Raster.h"
#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Image HDU reader-writer.
 * @details
 * The Fits format can accommodate images of any dimension.
 * EL_FitsIO stores them in n-dimensional containers: Raster objects.
 * This is visible in the ImageHdu services as a template parameter `n`.
 *
 * When the dimension is known at compile time and fixed,
 * it is recommended to specify it (`n > 0`),
 * which allows for a few internal optimizations
 * and brings safety as the code is less error-prone with fewer degrees of freedom.
 *
 * The zero vector space (`n = 0`) is supported;
 * It is used to represent HDUs with no data, as recommended in the Fits standard.
 *
 * When the dimension is unknown at compile time, or is expected to change
 * (e.g. 2D images to be merged as a 3D image),
 * special value `n = -1` can be specified.
 * In this case, the dimension is read in the Fits file,
 * but the user should be careful to handle all possible values.
 * @see
 * RecordHdu for services to read and write records.
 * @see
 * Position for details on the handling of fixed- and variable-dimension arrays.
 * @see
 * EL_FITSIO_FOREACH_RASTER_TYPE and program EL_FitsIO_PrintSupportedTypes for the list of supported pixel types.
 */
class ImageHdu : public RecordHdu {

public:
  /// @cond INTERNAL

  /**
   * @see RecordHdu
   */
  ImageHdu(Token, fitsfile*& fptr, long index);

  /**
   * @see RecordHdu
   */
  ImageHdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  virtual ~ImageHdu() = default;

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
   * @copydoc RecordHdu::readCategory
   */
  HduCategory readCategory() const override;

  /**
   * @brief Redefine the image shape and type.
   */
  template <typename T, long n = 2>
  void updateShape(const Position<n>& shape) const;

  /**
   * @brief Read the Raster.
   */
  template <typename T, long n = 2>
  VecRaster<T, n> readRaster() const;

  /**
   * @brief Write the Raster.
   */
  template <typename T, long n = 2>
  void writeRaster(const Raster<T, n>& raster) const;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_IMAGEHDU_IMPL
#include "EL_FitsFile/impl/ImageHdu.hpp"
#undef _EL_FITSFILE_IMAGEHDU_IMPL
/// @endcond

#endif
