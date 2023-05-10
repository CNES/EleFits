// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSION_H
#define COMPRESSION_H

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
 * @brief Types for quantize level (for floating point algos) & scaling factor (for HCompress).
 */
enum class FactorType {
  Absolute,
  Relative
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
   * @brief Get the of level (Absolute, Relative)
   */
  FactorType type() const;

private:
  Quantification(float qlevel, FactorType qType);

  float m_level;
  FactorType m_type;
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
   * @brief Get the type of scaling (Absolute, Relative)
   */
  FactorType type() const;

private:
  Scale(float factor, FactorType sType);

  float m_factor;
  FactorType m_type;
};

/**
 * @brief Base class for compression algorithms.
 */
class Algo {

  friend class ImageHdu;

public:
  /**
   * @brief Destructor.
   */
  virtual ~Algo() = default;

protected:
  virtual void compress(fitsfile* fptr) const = 0;
};

template <typename TDerived, long N>
class AlgoMixin : Algo {

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
  void set(Dithering dither);
  void set(Quantification quantize);
  void enableLossyInt();
  void disableLossyInt();
  Dithering dither() const;
  Quantification quantize() const;
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
  None();
  ~None();

private:
  const Euclid::Fits::Position<0> none_shape;
};

/**
 * @brief The Rice algorithm.
 */
template <long N>
class Rice : public FloatAlgo<Rice<N>, N> {

public:
  Rice(const Euclid::Fits::Position<N> shape);
  ~Rice();
};

/**
 * @brief The HCompress algorithm.
 */
template <long N>
class HCompress : public FloatAlgo<HCompress<N>, N> {

public:
  HCompress(const Euclid::Fits::Position<N> shape);
  ~HCompress();

  void set(Scale scale);
  void enableSmoothing();
  void disableSmoothing();
  float scale() const;
  FactorType scaleType() const;
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
  Plio(const Euclid::Fits::Position<N> shape);
  ~Plio();
};

/**
 * @brief The Gzip algorithm.
 */
template <long N>
class Gzip : public FloatAlgo<Gzip<N>, N> {

public:
  Gzip(const Euclid::Fits::Position<N> shape);
  ~Gzip();
};

/**
 * @brief The Gzip algorithm applied to "shuffled" pixel values,
 * where most significant bytes of each value appear first.
 */
template <long N>
class ShuffledGzip : public FloatAlgo<ShuffledGzip<N>, N> {

public:
  ShuffledGzip(const Euclid::Fits::Position<N> shape);
  ~ShuffledGzip();
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