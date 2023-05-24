// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Position.h"

#include <string>

namespace Euclid {
namespace Fits {
namespace Compression {

/**
 * @brief Dithering methods for quantification.
 */
enum class Dithering {
  None, ///< Do not dither any pixel
  NonZeroPixel, ///< Dither only non-zero pixels
  EveryPixel ///< Dither all pixels
};

/**
 * @brief Quantification of floating-types for FloatAlgo
 * FIXME: add dither_offset
*/
class Quantification {

public:
  /**
   * @brief Create quantification with default parameters
   */
  Quantification();

  /**
   * @brief Set globally the quantize level to the given value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  void absoluteLevel(float qlevel);

  /**
   * @brief Set tile-wise the quantize level to: tile RMS_noise * 1/value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  void relativeLevel(float qlevel);

  /**
   * @brief Set the dithering method for the quantification.
   */
  void dithering(Dithering);

  void enableLossyInt();
  void disableLossyInt();

  /**
   * @brief Get the quantize level
   */
  float level() const;

  /**
   * @brief Get if quantization is absolute or relative to noise.
   * @details
   * Always considered relative for qlevel of 0.
   */
  bool isAbsolute() const;

  /**
   * @brief Get the dithering method for the quantification.
   */
  Dithering dithering() const;

  bool hasLossyInt() const;

private:
  // Quantification(float qlevel);

  float m_level;
  Dithering m_dithering;
  bool m_lossyInt;
};

/**
 * @brief Scale factor associated to the HCompress algorithm
*/
class Scale {

public:
  /**
   * @brief Set globally the scaling factor to the given value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  static Scale absolute(float factor);

  /**
   * @brief Set tile-wise the scaling factor to: tile RMS_noise * 1/value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  static Scale relativeToNoise(float factor);

  /**
   * @brief Get the scaling factor
   */
  float factor() const;

  /**
   * @brief Get if the scaling is absolute or relative to noise
   * @details
   * Always considered relative for scale of 0.
   */
  bool isAbsolute() const;

private:
  Scale(float factor);

  float m_factor;
};

/**
 * @brief Base class for compression algorithms.
 * FIXME: friend class to remove or change depending on who will handle compress()
 */
class Algo {

  friend class ImageHdu;

public:
  Algo() = default;
  ELEFITS_VIRTUAL_DTOR(Algo)
  ELEFITS_COPYABLE(Algo)
  ELEFITS_MOVABLE(Algo)

protected:
  virtual void compress(void* fptr) const = 0;
};

/**
 * @brief Generic Algo class holding the tiling shape.
 * @details
 * Tiling shape is represented as a Position<N>.
 * The maximum dimension possible with cfitsion is equal to 6 (MAX_COMPRESS_DIM).
 * The value of N must therefore not exceed 6.
 * TODO: investigate how different tiling behaviours can be set at construction
 * FIXME: add boolean attribute huge for fits_set_huge_hdu
 */
template <long N, typename TDerived>
class AlgoMixin : Algo {

public:
  ELEFITS_VIRTUAL_DTOR(AlgoMixin)
  ELEFITS_COPYABLE(AlgoMixin)
  ELEFITS_MOVABLE(AlgoMixin)

  void quantize(Quantification quantize);
  const Position<N>& shape() const;
  const Quantification& quantize() const;

protected:
  void compress(void* fptr) const final;

  /**
   * @brief Constructor.
   */
  AlgoMixin<N, TDerived>(Position<N> shape);

private:
  /**
   * @brief The shape of the tiles.
   */
  Position<N> m_shape;

  /**
   * @brief Stores all parameters concerning quantification for floating-point algorithms
   */
  Quantification m_quantize;
};

/**
 * @brief No compression
 */
class None : public AlgoMixin<0, None> {

public:
  ELEFITS_VIRTUAL_DTOR(None)
  ELEFITS_COPYABLE(None)
  ELEFITS_MOVABLE(None)

  None();
};

/**
 * @brief The Rice algorithm.
 */
template <long N>
class Rice : public AlgoMixin<N, Rice<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Rice)
  ELEFITS_COPYABLE(Rice)
  ELEFITS_MOVABLE(Rice)

  Rice(const Position<N> shape);
};

/**
 * @brief The HCompress algorithm.
 */
class HCompress : public AlgoMixin<2, HCompress> {

public:
  ELEFITS_VIRTUAL_DTOR(HCompress)
  ELEFITS_COPYABLE(HCompress)
  ELEFITS_MOVABLE(HCompress)

  HCompress(const Position<2> shape);

  void scale(Scale scale);
  void enableSmoothing();
  void disableSmoothing();
  const Scale& scale() const;
  bool isSmooth() const;

private:
  Scale m_scale;
  bool m_smooth;
};

/**
 * @brief The Plio algorithm.
 */
template <long N>
class Plio : public AlgoMixin<N, Plio<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Plio)
  ELEFITS_COPYABLE(Plio)
  ELEFITS_MOVABLE(Plio)

  Plio(const Position<N> shape);
};

/**
 * @brief The Gzip algorithm.
 */
template <long N>
class Gzip : public AlgoMixin<N, Gzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  Gzip(const Position<N> shape);
};

/**
 * @brief The Gzip algorithm applied to "shuffled" pixel values,
 * where most significant bytes of each value appear first.
 */
template <long N>
class ShuffledGzip : public AlgoMixin<N, ShuffledGzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  ShuffledGzip(const Position<N> shape);
};

} // namespace Compression
} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define COMPRESSION_IMPL
#include "EleFitsData/impl/Compression.hpp"
#undef COMPRESSION_IMPL
/// @endcond

#endif