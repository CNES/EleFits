// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Position.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Fits {
namespace Compression {

/**
 * @brief Dithering methods.
 * TOFIX: what about function fits_set_quantize_method() -> seems exact same as set_quantize_dither()
 * TOFIX: include Dithering in Quantification ?
 * TOFIX: add dither_offset
 */
enum class Dithering {
  NoDithering, ///< Do not dither any pixel
  NonZeroPixelDithering, ///< Dither only non-zero pixels
  EveryPixelDithering ///< Dither all pixels
};

/**
 * @brief Quantification of floating-types for FloatAlgo
*/
class Quantification {

public:
  /**
   * @brief Set globally the quantize level to the given value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  static Quantification absolute(float qlevel);

  /**
   * @brief Set tile-wise the quantize level to: tile RMS_noise * 1/value.
   * @details
   * A non-zero scale enables lossy compression of integer data.
   */
  static Quantification relativeToNoise(float qlevel);

  /**
   * @brief Get the quantize level
   */
  float level() const;

  /**
   * @brief Get if quantization is absolute or relative to noise
   * @details
   * Always considered relative for qlevel of 0.
   */
  bool isAbsolute() const;

private:
  Quantification(float qlevel);

  float m_level;
};

/**
 * @brief Scale factor associated to the HCompress algorithm
 * TOFIX: factorize Scale and Quantification into FactorParameter ?
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
   * @brief Set tile-wise the scaling factor to: tile RMS_noise * value.
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
 */
class Algo {

  friend class ImageHdu;

public:
  ELEFITS_VIRTUAL_DTOR(Algo)

protected:
  virtual void compress(fitsfile* fptr) const = 0;
};

template <typename TDerived, long N>
class AlgoMixin : Algo {

public:
  ELEFITS_VIRTUAL_DTOR(AlgoMixin)

protected:
  void compress(fitsfile* fptr) const override;

  /**
   * @brief Constructor.
   */
  AlgoMixin<TDerived, N>(const Euclid::Fits::Position<N> shape);

private:
  /**
   * @brief The shape of the tiles.
   * The maximum dimension possible with cfitsion is equal to 6 (MAX_COMPRESS_DIM)
   */
  Euclid::Fits::Position<N> m_shape;
  // TOFIX: add huge for fits_set_huge_hdu
};

/**
 * @brief Compression algorithms for floating-point data.
 */
template <typename TDerived, long N>
class FloatAlgo : public AlgoMixin<TDerived, N> {

public:
  ELEFITS_VIRTUAL_DTOR(FloatAlgo)

  void dither(Dithering dither);
  void quantize(Quantification quantize);
  void enableLossyInt();
  void disableLossyInt();
  Dithering dither() const;
  const Quantification& quantize() const;
  bool lossyInt() const;

protected:
  FloatAlgo<TDerived, N>(const Euclid::Fits::Position<N> shape);

  void compress(fitsfile* fptr) const override;

protected:
  Quantification m_quantize;
  Dithering m_dither;
  bool m_lossyInt;
};

/**
 * @brief No compression
 */
class None : public AlgoMixin<None, 0> {

public:
  ELEFITS_VIRTUAL_DTOR(None)
  ELEFITS_COPYABLE(None)
  ELEFITS_MOVABLE(None)

  None();

private:
  const Euclid::Fits::Position<0> none_shape;
};

/**
 * @brief The Rice algorithm.
 */
template <long N>
class Rice : public FloatAlgo<Rice<N>, N> {

public:
  ELEFITS_VIRTUAL_DTOR(Rice)
  ELEFITS_COPYABLE(Rice)
  ELEFITS_MOVABLE(Rice)

  Rice(const Euclid::Fits::Position<N> shape);
};

/**
 * @brief The HCompress algorithm.
 */
template <long N>
class HCompress : public FloatAlgo<HCompress<N>, N> {

public:
  ELEFITS_VIRTUAL_DTOR(HCompress)
  ELEFITS_COPYABLE(HCompress)
  ELEFITS_MOVABLE(HCompress)

  HCompress(const Euclid::Fits::Position<N> shape);

  void scale(Scale scale);
  void enableSmoothing();
  void disableSmoothing();
  const Scale& scale() const;
  bool smooth() const;

private:
  Scale m_scale;
  bool m_smooth;
};

/**
 * @brief The Plio algorithm.
 */
template <long N>
class Plio : public AlgoMixin<Plio<N>, N> {

public:
  ELEFITS_VIRTUAL_DTOR(Plio)
  ELEFITS_COPYABLE(Plio)
  ELEFITS_MOVABLE(Plio)

  Plio(const Euclid::Fits::Position<N> shape);
};

/**
 * @brief The Gzip algorithm.
 */
template <long N>
class Gzip : public FloatAlgo<Gzip<N>, N> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  Gzip(const Euclid::Fits::Position<N> shape);
};

/**
 * @brief The Gzip algorithm applied to "shuffled" pixel values,
 * where most significant bytes of each value appear first.
 */
template <long N>
class ShuffledGzip : public FloatAlgo<ShuffledGzip<N>, N> {

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  ShuffledGzip(const Euclid::Fits::Position<N> shape);
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