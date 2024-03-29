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
 * Linx::Position for details on the handling of fixed- and variable-dimension arrays.
 * @see
 * ELEFITS_FOREACH_RASTER_TYPE and program EleFitsPrintSupportedTypes for the list of supported pixel types.
 */
class ImageHdu : public Hdu {
public:

  /**
   * @brief A structure which holds everything known at image extension initialization.
   */
  template <typename T>
  struct Initializer {
    /**
     * @brief The extension index.
     */
    Linx::Index index;

    /**
     * @brief The extension name, or an empty string.
     */
    const std::string& name;

    /**
     * @brief The possibly empty sequence of records.
     */
    const RecordSeq& records;

    /**
     * @brief The shape.
     */
    Linx::Position<-1> shape;

    /**
     * @brief The data, if any.
     */
    const T* data;
  };

  /// @group_construction

  /// @cond

  /**
   * @see Hdu
   */
  ImageHdu(Token, fitsfile*& fptr, Linx::Index index, HduCategory status = HduCategory::Untouched);

  /**
   * @see Hdu
   */
  ImageHdu();

  /// @endcond

  // FIXME copyable?

  LINX_VIRTUAL_DTOR(ImageHdu)

  /**
   * @brief Copy the contents of another image HDU.
   */
  const ImageHdu& operator=(const ImageHdu& rhs) const;

  /// @group_properties

  /**
   * @copydoc Hdu::category
   */
  HduCategory category() const override;

  /**
   * @brief Check whether the HDU is compressed.
   */
  bool is_compressed() const;

  /// @group_elements

  /**
   * @brief Access the data unit to read and write the raster.
   * @see ImageRaster
   */
  const ImageRaster& raster() const;

  /// @group_operations

  /**
   * @copybrief ImageRaster::read_typeid
   */
  const std::type_info& read_typeid() const;

  /**
   * @copybrief ImageRaster::read_size
   */
  Linx::Index read_size() const;

  /**
   * @copybrief ImageRaster::read_shape
   */
  template <Linx::Index N = 2>
  Linx::Position<N> read_shape() const;

  /**
   * @brief Read the compression parameters.
   */
  std::unique_ptr<Compression> read_compression() const;

  /**
   * @brief Redefine the image shape and type.
   */
  template <typename T, Linx::Index N = 2>
  void update_type_shape(const Linx::Position<N>& shape) const;

  /**
   * @brief Read the Raster.
   */
  template <typename T, Linx::Index N = 2>
  Linx::Raster<T, N> read_raster() const;

  /**
   * @brief Write the Raster.
   */
  template <typename TRaster>
  void write_raster(const TRaster& data) const;

  /// @}

private:

  /**
   * @brief The data unit handler.
   */
  ImageRaster m_raster;
};

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITS_IMAGEHDU_IMPL
#include "EleFits/impl/ImageHdu.hpp"
#undef _ELEFITS_IMAGEHDU_IMPL
/// @endcond

#endif
