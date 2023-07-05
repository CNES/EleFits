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

/// @cond
// Forward declaration for friendship in Algo
class MefFile;
/// @endcond

namespace Compression {

class Factor {

public:
  enum Type {
    None,
    Absolute,
    Relative
  };

  static inline Factor none();
  static inline Factor absolute(float value);
  static inline Factor relative(float value);

  inline Factor::Type type() const;

  inline float value() const;

  inline bool operator==(const Factor& f2) const;

private:
  inline Factor(float value);

  float m_value;
};

/**
 * @brief Dithering methods for quantization.
 */
enum class Dithering {
  None, ///< Do not dither any pixel
  NonZeroPixel, ///< Dither only non-zero pixels
  EveryPixel ///< Dither all pixels
};

/**
 * @brief Quantization of floating-types for FloatAlgo
 * TODO: add multi argument constructor
 * FIXME: add dither_offset
*/
class Quantization {

public:
  /**
   * @brief Constructor.
   */
  inline Quantization(Factor level = Factor::none());

  /**
   * @brief Set tile-wise the quantize level
   * @details
   * A relative factor will set the quantize level to: tile RMS_noise * 1/value.
   * An absolute factor will set globally the quantize level to the given value.
   * A none Factor disables lossy compression of integer data.
   */
  inline Quantization& level(Factor level);

  /**
   * @brief Set the dithering method for the quantization.
   */
  inline Quantization& dithering(Dithering);

  inline Quantization& enableLossyInt();

  inline Quantization& disableLossyInt();

  /**
   * @brief Get the quantize level
   */
  inline const Factor& level() const;

  /**
   * @brief Get the dithering method for the quantization.
   */
  inline Dithering dithering() const;

  inline bool hasLossyInt() const;

  inline bool operator==(const Quantization& q2) const;

private:
  Factor m_level;
  Dithering m_dithering;
  bool m_lossyInt;
};

/**
 * @brief Base class for compression algorithms.
 * FIXME: friend class to be removed or changed depending on who will handle compress()
 */
class Algo {

  friend class Euclid::Fits::MefFile;

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
 * 
 * Setting tiling policies: for each dimension;
 * -1 means that the maximum number of pixel in this dimension will be used.
 * n>0 means that the tiling will be of size n for this dimension.
 * For instance:
 * - a shape of -Fits::Position<N>::one() will create tiles of the maximum size (all pixels in 6 dimensions)
 * - a shape of Fits::Position<6>::({-1,1,1,1,1,1}) will create row-wise tiling
 * - a shape of Fits::Position<6>::({30,20,10,1,1,1}) will create a tiling of size 30 x 20 x 10
 * TODO: investigate how different tiling behaviours can be set at construction
 * FIXME: variable tiling dims with N=-1 currently not supported
 * FIXME: add boolean attribute huge for fits_set_huge_hdu
 */
template <long N, typename TDerived>
class AlgoMixin : public Algo {

  friend class Euclid::Fits::MefFile;

public:
  ELEFITS_VIRTUAL_DTOR(AlgoMixin)
  ELEFITS_COPYABLE(AlgoMixin)
  ELEFITS_MOVABLE(AlgoMixin)

  TDerived& quantize(Quantization quantize);

  const Position<N>& shape() const;

  const Quantization& quantize() const;

protected:
  void compress(void* fptr) const final;
  // FIXME define the function here instead of in the wrapper
  // The function is not used anyway, and this simplifies compilation
  // => Change compress(fitsfile*, TDerived) into compress(void*, TDerived)
  // or forward declare fitsfile

  /**
   * @brief Constructor (specify the compression tiling shape).
   */
  AlgoMixin(Position<N> shape);

  Position<N> rowWiseTiling() const;

private:
  /**
   * @brief The shape of the tiles.
   */
  Position<N> m_shape;

  /**
   * @brief Stores all parameters concerning quantization for floating-point algorithms
   */
  Quantization m_quantize;
};

/**
 * @brief No compression
 */
class None : public AlgoMixin<0, None> {

public:
  ELEFITS_VIRTUAL_DTOR(None)
  ELEFITS_COPYABLE(None)
  ELEFITS_MOVABLE(None)

  inline None();
};

/**
 * @brief The Rice algorithm.
 */
template <long N = 6>
class Rice : public AlgoMixin<N, Rice<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Rice)
  ELEFITS_COPYABLE(Rice)
  ELEFITS_MOVABLE(Rice)

  /**
   * @brief Default constructor (tiling set to row-wise)
   */
  Rice();
  /**
   * @brief Construct by scpecifying compression tiling
   * @see AlgoMixin
   */
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

  /**
   * @brief Default constructor (using the whole 2D images as tiling)
   */
  inline HCompress();
  /**
   * @brief Construct by scpecifying 2D compression tiling
   * @see AlgoMixin
   */
  inline HCompress(const Position<2> shape);

  /**
   * @brief Set tile-wise scaling for hcompress.
   * @details
   * A relative factor will set the scaling factor to: tile RMS_noise * 1/value.
   * An absolute factor will set globally the scaling factor to the given value.
   * A none factor disables scaling.
   */
  inline HCompress& scale(Factor scale);

  inline HCompress& enableSmoothing();

  inline HCompress& disableSmoothing();

  inline const Factor& scale() const;

  inline bool isSmooth() const;

private:
  Factor m_scale;
  bool m_smooth;
};

/**
 * @brief The Plio algorithm.
 */
template <long N = 6>
class Plio : public AlgoMixin<N, Plio<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Plio)
  ELEFITS_COPYABLE(Plio)
  ELEFITS_MOVABLE(Plio)

  /**
   * @brief Default constructor (tiling set to row-wise)
   */
  Plio();

  /**
   * @brief Construct by scpecifying compression tiling
   * @see AlgoMixin
   */
  Plio(const Position<N> shape);
};

/**
 * @brief The Gzip algorithm.
 */
template <long N = 6>
class Gzip : public AlgoMixin<N, Gzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  /**
   * @brief Default constructor (tiling set to row-wise)
   */
  Gzip();
  /**
   * @brief Construct by scpecifying compression tiling
   * @see AlgoMixin
   */
  Gzip(const Position<N> shape);
};

/**
 * @brief The Gzip algorithm applied to "shuffled" pixel values, where most significant bytes of each value appear first.
 */
template <long N = 6>
class ShuffledGzip : public AlgoMixin<N, ShuffledGzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  /**
   * @brief Default constructor (tiling set to row-wise)
   */
  ShuffledGzip();
  /**
   * @brief Construct by scpecifying compression tiling
   * @see AlgoMixin
   */
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