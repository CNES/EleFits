// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_IMAGEHDU_H
#define _ELEFITS_IMAGEHDU_H

#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/Hdu.h"
#include "EleFits/ImageRaster.h"
#include "EleFitsData/Compression.h"
#include "EleFitsData/Raster.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_handlers
 * @brief Image HDU reader-writer.
 * @details
 * The FITS format can accommodate images of any dimension.
 * EleFits stores them in _n_-dimensional containers: Raster objects.
 * This is visible in the ImageHdu services as a template parameter `N`.
 *
 * When the dimension is known at compile time and fixed,
 * it is recommended to specify it (`N > 0`),
 * which allows for a few internal optimizations
 * and brings safety as the code is less error-prone with fewer degrees of freedom.
 *
 * The zero vector space (`N = 0`) is supported;
 * It is used to represent HDUs with no data, as recommended in the FITS standard.
 *
 * When the dimension is unknown at compile time, or is expected to change
 * (e.g. 2D images to be merged as a 3D image),
 * special value `N = -1` can be specified.
 * In this case, the dimension is read in the FITS file,
 * but the user should be careful to handle all possible values.
 * @see
 * Hdu for services to read and write records.
 * @see
 * Position for details on the handling of fixed- and variable-dimension arrays.
 * @see
 * ELEFITS_FOREACH_RASTER_TYPE and program EleFitsPrintSupportedTypes for the list of supported pixel types.
 */
class ImageHdu : public Hdu {

public:
  /// @cond INTERNAL

  /**
   * @see Hdu
   */
  ImageHdu(Token, fitsfile*& fptr, long index, HduCategory status = HduCategory::Untouched);

  /**
   * @see Hdu
   */
  ImageHdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  virtual ~ImageHdu() = default;

  /**
   * @brief Access the data unit to read and write the raster.
   * @see ImageRaster
   */
  const ImageRaster& raster() const;

  /**
   * @brief Read the image pixel value type.
   */
  const std::type_info& readTypeid() const;

  /**
   * @brief Read the image cfitsio bitpix value.
   */
  long readBitpix() const;

  /**
   * @brief Read the number of pixels in the image.
   */
  long readSize() const;

  /**
   * @brief Read the image shape.
   */
  template <long N = 2>
  Position<N> readShape() const;

  /**
   * @copydoc Hdu::readCategory
   */
  HduCategory readCategory() const override;

  /**
   * @brief Check whether the HDU is compressed.
   */
  bool isCompressed() const;

  /**
   * @brief Read the compression parameters.
   */
  std::unique_ptr<Compression> readCompression() const;

  /**
   * @brief Redefine the image shape and type.
   */
  template <typename T, long N = 2>
  void updateShape(const Position<N>& shape) const;

  /**
   * @brief Read the Raster.
   */
  template <typename T, long N = 2>
  VecRaster<T, N> readRaster() const;

  /**
   * @brief Write the Raster.
   */
  template <typename TRaster>
  void writeRaster(const TRaster& data) const;

private:
  /**
   * @brief The data unit handler.
   */
  ImageRaster m_raster;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_IMAGEHDU_IMPL
#include "EleFits/impl/ImageHdu.hpp"
#undef _ELEFITS_IMAGEHDU_IMPL
/// @endcond

#endif
