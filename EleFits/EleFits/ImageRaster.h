// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_IMAGERASTER_H
#define _ELEFITS_IMAGERASTER_H

#include "EleFitsData/Raster.h"

#include <fitsio.h>
#include <functional>

namespace Fits {

/**
 * @ingroup image_handlers
 * @brief Reader-writer for the image data unit.
 * 
 * This handler provides methods to access image metadata (image-related keyword records) and data.
 * 
 * Reading methods either return a `Raster` or fill an existing `Raster`.
 * 
 * Data can be read and written region-wise.
 * 
 * @see Linx
 */
class ImageRaster {
private:

  friend class ImageHdu;

  /**
   * @brief Constructor.
   */
  ImageRaster(fitsfile*& fptr, std::function<void(void)> touch, std::function<void(void)> edit);

public:

  /// @group_properties

  /**
   * @brief Read the image pixel value type.
   */
  const std::type_info& read_typeid() const;

  /**
   * @brief Read the `BITPIX` or `ZBITPIX` value.
   */
  Linx::Index read_bitpix() const;

  /**
   * @brief Read the number of pixels in the image.
   */
  Linx::Index read_size() const;

  /**
   * @brief Read the image shape.
   */
  template <Linx::Index N = 2>
  Linx::Position<N> read_shape() const;

  /**
   * @brief Update the image shape.
   */
  template <Linx::Index N = 2>
  void update_shape(Linx::Position<N> shape) const;

  /**
   * @brief Update the image type and shape.
   */
  template <typename T, Linx::Index N = 2>
  void update_type_shape(Linx::Position<N> shape) const;

  /// @}
  /**
   * @name Read the whole data unit
   */
  /// @{

  /**
   * @brief Read the whole data unit as a new raster.
   * 
   * There are several options to read the whole data unit:
   * - as a new `Raster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Patch` object.
   * 
   * In the latter two cases, the raster or patch is assumed to already have a conforming shape.
   */
  template <typename T, Linx::Index N = 2>
  Linx::Raster<T, N> read() const;

  /**
   * @brief Read the whole data unit into an existing raster or patch.
   * @copydetails read()
   */
  template <typename TOut>
  void read_to(TOut& out) const;

  /// @}
  /**
   * @name Read a region of the data unit
   */
  /// @{

  /**
   * @brief Read a region as a new raster.
   * @tparam T The desired raster type
   * @tparam M The desired raster dimension, which can be smaller than the data dimension in file
   * @param region The in-file region
   * @param front The front position in the file
   * @param out The destination raster or patch
   * 
   * There are several options to read a region of the data unit:
   * - as a new `Raster` object;
   * - by filling an existing `Raster` object;
   * - by filling an existing `Patch` object.
   * In the latter two cases, only the front position of the region to be read is given,
   * and the shape of the region is deduced from that of `out`.
   * 
   * For example, to read the HDU region from position (50, 80) to position (100, 120)
   * into an existing raster from position (25, 40) to position (75, 80), do:
   * \code
   * const Position<2> front {50, 80};
   * auto patch = raster({{25, 40}, {75, 80}});
   * image.read_region_to(front, patch);
   * \endcode
   * where `image` is the `ImageRaster` and `raster` is the `Raster`.
   */
  template <typename T, Linx::Index M, Linx::Index N>
  Linx::Raster<T, M> read_region(const Linx::Box<N>& region) const;

  /**
   * @brief Read a region of the data unit into a region of an existing `Raster`.
   * @copydetails read_region()
   */
  template <Linx::Index N, typename TOut>
  void read_region_to(Linx::Position<N> front, TOut& out) const;

  /// @}
  /**
   * @name Write the whole data unit
   */
  /// @{

  /**
   * @brief Write the whole data unit.
   * @param in The raster or patch to be written
   */
  template <typename TIn>
  void write(const TIn& in) const;

  /// @}
  /**
   * @name Write a region of the data unit
   */
  /// @{

  /**
   * @brief Write a raster at a given position of the data unit.
   * @param front The in-file front position
   * @param in The raster or patch to be written
   * 
   * Note that the raster dimension can be lower than the HDU dimension.
   * For example, it is possible to write a 2D raster in a 3D HDU.
   * \code
   * // Write the 3rd plane of raster into the 5th plane of the HDU
   * du.write_region<3>({0, 0, 4}, raster.section(2));
   * \endcode
   */
  template <Linx::Index N, typename TIn>
  void write_region(Linx::Position<N> front, const TIn& in) const;

  /**
   * @brief Update the type and shape and write somr raster or patch.
   */
  template <typename TIn>
  void update(const TIn& in) const;

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

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITS_IMAGERASTER_IMPL
#include "EleFits/impl/ImageRaster.hpp"
#undef _ELEFITS_IMAGERASTER_IMPL
/// @endcond

#endif
