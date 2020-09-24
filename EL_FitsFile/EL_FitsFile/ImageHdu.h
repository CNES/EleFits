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
 */
class ImageHdu : public RecordHdu {

public:
  /**
   * @brief Constructor.
   * @warning
   * You should probably not instantiate ImageHdus yourself,
   * but use the dedicated MefFile creation method
   * MefFile::initImageExt or MefFile::assignImageExt.
   * @todo
   * The constructor should be protected, with MefFile a friend of the class.
   */
  ImageHdu(fitsfile *&fptr, long index);

  /**
   * @brief Destructor.
   */
  virtual ~ImageHdu() = default;

  /**
   * @brief Read the image shape.
   */
  template <long n = 2>
  Position<n> readShape() const;

  /**
   * @brief Redefine the image shape and type.
   */
  template <typename T, long n = 2>
  void updateShape(const Position<n> &shape) const;

  /**
   * @brief Read the Raster.
   */
  template <typename T, long n = 2>
  VecRaster<T, n> readRaster() const;

  /**
   * @brief Write the Raster.
   */
  template <typename T, long n = 2>
  void writeRaster(const Raster<T, n> &raster) const;
};

} // namespace FitsIO
} // namespace Euclid

#define _EL_FITSFILE_IMAGEHDU_IMPL
#include "EL_FitsFile/impl/ImageHdu.hpp"
#undef _EL_FITSFILE_IMAGEHDU_IMPL

#endif
