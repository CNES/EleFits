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

/**
 * @brief FITS-internal, tiled compression of Image HDUs.
 */
namespace Compression {

/**
 * @brief Create a rowwise tiling.
 * @param rowCount The number of rows per tile
 */
template <long N>
Position<N> rowwiseTiling(long rowCount = 1) {
  Position<N> out(N == -1 ? 2 : N);
  std::fill(out.begin(), out.end(), 1);
  out[0] = -1;
  out[1] = rowCount;
  return out;
}

/**
 * @brief Create a whole-data array tiling.
 */
template <long N>
Position<N> wholeDataTiling() {
  return Position<N>::max();
}

/**
 * @brief A factor which can be absolute or relative to the noise level in each tile.
 * 
 * A `relative()` factor yields: `absolute() = RMS_noise / relative()`.
 * A `none()` factor can be used to disable the feature it represents.
 */
class Factor {

public:
  /**
   * @brief The type of factor.
   */
  enum Type {
    None, ///< Disable feature
    Absolute, ///< Absolute value
    Relative ///< Tile-relative value
  };

  /**
   * @brief Create a disabled factor.
   */
  static inline Factor none();

  /**
   * @brief Create an absolute factor.
   */
  static inline Factor absolute(float value);

  /**
   * @brief Create a relative factor.
   */
  static inline Factor relative(float value);

  /**
   * @brief Get the factor type.
   */
  inline Factor::Type type() const;

  /**
   * @brief Get the factor value.
   */
  inline float value() const;

  /**
   * @brief Check whether two factors are equal.
   */
  inline bool operator==(const Factor& rhs) const;

private:
  /**
   * @brief Create a factor.
   */
  inline Factor(float value);

  /**
   * @brief The factor value, which encodes the type as its sign.
   */
  float m_value;
};

/**
 * @brief Quantization dithering methods.
 */
enum class Dithering {
  None, ///< Do not dither any pixel
  NonZeroPixel, ///< Dither only non-zero pixels
  EveryPixel ///< Dither all pixels
};

/**
 * @brief Quantization of floating-point pixels.
 */
class Quantization {

public:
  /**
   * @brief Constructor.
   * 
   * Integer data compression is lossless by default.
   */
  inline Quantization(Factor level = Factor::relative(4), Dithering method = Dithering::EveryPixel);

  /**
   * @brief Set the quantization level.
   */
  inline Quantization& level(Factor level);

  /**
   * @brief Set the dithering method.
   */
  inline Quantization& dithering(Dithering method);

  /**
   * @brief Enable lossy compression of integer data.
   * 
   * This is accomplished by considering the values as floating points and then applying quantization.
   */
  inline Quantization& enableLossyInt();

  /**
   * @brief Disable lossy compression of integer data.
   */
  inline Quantization& disableLossyInt();

  /**
   * @brief Get the quantization level
   */
  inline const Factor& level() const;

  /**
   * @brief Get the dithering method for the quantization.
   */
  inline Dithering dithering() const;

  /**
   * @brief Check whether lossy integral compression is enabled.
   */
  inline bool hasLossyInt() const;

  /**
   * @brief Check whether two quatizations are equal.
   */
  inline bool operator==(const Quantization& rhs) const;

private:
  /**
   * @brief The quantization level.
   */
  Factor m_level;

  /**
   * @brief The quantization dithering method.
   */
  Dithering m_dithering;

  /**
   * @brief The lossy integral compression flag.
   */
  bool m_lossyInt;

  // FIXME handle dither offset
};

/**
 * @brief Interface for compression algorithms.
 */
class Algo {

  friend class Euclid::Fits::MefFile; // TODO rm if/when possible

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
template <long N, typename TDerived> // FIXME rm N?
class AlgoMixin : public Algo {

public:
  /**
   * @brief The dimension parameter of the tiling.
   */
  static constexpr long Dim = N;

  ELEFITS_VIRTUAL_DTOR(AlgoMixin)
  ELEFITS_COPYABLE(AlgoMixin)
  ELEFITS_MOVABLE(AlgoMixin)

  /**
   * @brief Get the tiling.
   */
  const Position<N>& shape() const;

  /**
   * @brief Get the quantization.
   */
  const Quantization& quantization() const; // FIXME move to children

  /**
   * @brief Set the tiling.
   */
  TDerived& shape(Position<N> shape);

  /**
   * @brief Set the quantization.
   */
  TDerived& quantization(Quantization quantization); // FIXME move to children

protected:
  /**
   * @brief Constructor.
   */
  AlgoMixin(Position<N> shape);

  /**
   * @brief Dependency inversion to call the wrapper's dispatch based on `TDerived`.
   */
  void compress(void* fptr) const final;
  // FIXME define the function here instead of in the wrapper
  // The function is not used anyway, and this simplifies compilation
  // => Change compress(fitsfile*, TDerived) into compress(void*, TDerived)
  // or forward declare fitsfile

private:
  /**
   * @brief The shape of the tiles.
   */
  Position<N> m_shape;

  /**
   * @brief Stores all parameters concerning quantization for floating-point algorithms
   */
  Quantization m_quantization;
};

/**
 * @brief No compression.
 */
class None : public AlgoMixin<0, None> {

public:
  ELEFITS_VIRTUAL_DTOR(None)
  ELEFITS_COPYABLE(None)
  ELEFITS_MOVABLE(None)

  /**
   * @brief Constructor.
   */
  inline None();
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

  /**
   * @brief Constructor.
   */
  Rice(Position<N> shape = rowwiseTiling<N>());
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
   * @brief Constructor.
   */
  inline HCompress(Position<2> shape = rowwiseTiling<2>(16));

  /**
   * @brief Get the scaling factor.
   */
  inline const Factor& scale() const;

  /**
   * @brief Check whether the image is smoothed at reading.
   */
  inline bool isSmooth() const;

  /**
   * @brief Set the scaling factor.
   */
  inline HCompress& scale(Factor scale);

  /**
   * @brief Enable image smoothing at reading.
   */
  inline HCompress& enableSmoothing();

  /**
   * @brief Disable image smoothing at reading.
   */
  inline HCompress& disableSmoothing();

private:
  /**
   * @brief The scale factor.
   */
  Factor m_scale;

  /**
   * @brief The smoothing flag.
   */
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

  /**
   * @brief Constructor
   */
  Plio(Position<N> shape = rowwiseTiling<N>());
};

/**
 * @brief The GZIP algorithm.
 */
template <long N>
class Gzip : public AlgoMixin<N, Gzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  /**
   * @brief Constructor
   */
  Gzip(Position<N> shape = rowwiseTiling<N>());
};

/**
 * @brief The GZIP algorithm applied to "shuffled" pixel values, where most significant bytes of each value appear first.
 * 
 * Generally, this algorithm is much more efficient in terms of compression factor than GZIP,
 * although it is a bit slower.
 */
template <long N>
class ShuffledGzip : public AlgoMixin<N, ShuffledGzip<N>> {

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  /**
   * @brief Constructor.
   */
  ShuffledGzip(Position<N> shape = rowwiseTiling<N>());
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