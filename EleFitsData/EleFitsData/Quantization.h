// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_QUANTIZATION_H
#define _ELEFITSDATA_QUANTIZATION_H

#include "EleFitsData/Position.h"
#include "EleFitsData/Scaling.h"

#include <memory>
#include <string>

namespace Euclid {
namespace Fits {

/**
 * @ingroup compression
 * @brief %Quantization parameters.
 * 
 * As opposed to CFITSIO, EleFits creates lossless algorithms by default,
 * including for floating point values.
 * Lossy compression has to be manually enabled by setting a non-null quantization level.
 * The level can be set either globally, as a double, or tile-wise, relative to the noise level:
 * 
 * \code
 * Quantization disabled;
 * Quantization absolute(1);
 * Quantization relative(Tile::rms / 4);
 * \endcode
 * 
 * When quantization is enabled, dithering is applied by default to all pixels.
 */
class Quantization {
public:

  /**
   * @brief Quantization dithering methods.
   */
  enum class Dithering {
    None, ///< Do not dither any pixel
    NonZeroPixel, ///< Dither only non-zero pixels
    EveryPixel ///< Dither all pixels
  };

  LINX_VIRTUAL_DTOR(Quantization)
  LINX_DEFAULT_COPYABLE(Quantization)
  LINX_DEFAULT_MOVABLE(Quantization)

  /**
     * @brief Default, lossless compression constructor.
     */
  inline explicit Quantization();

  /**
     * @brief Level-based constructor.
     * 
     * The default dithering for lossy compression is `Dithering::EveryPixel`.
     */
  inline explicit Quantization(Scaling level);

  /**
     * @brief Full constructor.
     */
  inline explicit Quantization(Scaling level, Dithering method);

  /**
     * @brief Get the quantization level
     */
  inline const Scaling& level() const;

  /**
     * @brief Get the dithering method for the quantization.
     */
  inline Dithering dithering() const;

  /**
     * @brief Check whether quantization is enabled.
     */
  inline explicit operator bool() const;

  /**
     * @brief Set the quantization level.
     */
  inline Quantization& level(Scaling level);

  /**
     * @brief Set the dithering method.
     */
  inline Quantization& dithering(Dithering method);

  /**
     * @brief Check whether two quatizations are equal.
     */
  inline bool operator==(const Quantization& rhs) const;

  /**
     * @brief Check whether two quatizations are different.
     */
  inline bool operator!=(const Quantization& rhs) const;

private:

  /**
     * @brief The quantization level.
     */
  Scaling m_level;

  /**
     * @brief The quantization dithering method.
     */
  Dithering m_dithering;

  // FIXME handle dither offset and seed
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_QUANTIZATION_IMPL
#include "EleFitsData/impl/Quantization.hpp"
#undef _ELEFITSDATA_QUANTIZATION_IMPL
/// @endcond

#endif