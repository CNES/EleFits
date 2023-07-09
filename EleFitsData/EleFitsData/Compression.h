// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Position.h"

#include <memory>
#include <string>

namespace Euclid {
namespace Fits {

/// @cond
// Forward declaration for friendship in Algo
class MefFile;
/// @endcond

/**
 * @ingroup image_compression
 * @brief FITS-internal, tiled compression of Image HDUs.
 */
namespace Compression {

/**
 * @relates Algo
 * @brief Create a rowwise tiling.
 * @param rowCount The number of rows per tile
 */
inline Position<-1> rowwiseTiling(long rowCount = 1) {
  return Position<-1> {-1, rowCount};
}

/**
 * @relates Algo
 * @brief Create a whole-data array tiling.
 */
inline Position<-1> maxTiling() {
  return Position<-1> {-1};
}

/**
 * @ingroup image_compression
 * @brief A parameter which can be absolute or relative to the noise level in each tile.
 */
// FIXME for q, absolute = RMS / relative
// but for s, absolue = RMS * relative
class Param {

public:
  /**
   * @brief The type of parameter.
   */
  enum Type {
    None, ///< Disable feature
    Absolute, ///< Absolute value
    Relative ///< Tile-relative value
  };

  /**
   * @brief Create a disabled parameter.
   */
  static inline Param none();

  /**
   * @brief Create an absolute parameter.
   */
  static inline Param absolute(double value);

  /**
   * @brief Create a relative parameter.
   */
  static inline Param relative(double value);

  /**
   * @brief Get the parameter type.
   */
  inline Param::Type type() const;

  /**
   * @brief Get the parameter value.
   */
  inline double value() const;

  /**
   * @brief Cast as a Boolean, i.e. `false` iff type is `None`.
  */
  inline operator bool() const;

  /**
   * @brief Check whether two factors are equal.
   */
  inline bool operator==(const Param& rhs) const;

  /**
   * @brief Check whether two factors are different.
   */
  inline bool operator!=(const Param& rhs) const;

private:
  /**
   * @brief Create a parameter.
   */
  inline explicit Param(double value);

  /**
   * @brief The parameter value, which encodes the type as its sign.
   */
  double m_value;
};

/**
 * @ingroup image_compression
 * @brief Quantization dithering methods.
 */
enum class Dithering {
  None, ///< Do not dither any pixel
  NonZeroPixel, ///< Dither only non-zero pixels
  EveryPixel ///< Dither all pixels
};

/**
 * @ingroup image_compression
 * @brief Quantization of pixels.
 * 
 * As opposed to CFITSIO, EleFits creates lossless algorithms by default,
 * including for floating point values.
 */
class Quantization {

public:
  /**
   * @brief Default, lossless compression constructor.
   */
  inline explicit Quantization();

  /**
   * @brief Level-based constructor.
   * 
   * The default dithering for lossy compression is `Dithering::EveryPixel`.
   */
  inline explicit Quantization(Param level);

  /**
   * @brief Full constructor.
   */
  inline explicit Quantization(Param level, Dithering method);

  /**
   * @brief Get the quantization level
   */
  inline const Param& level() const;

  /**
   * @brief Get the dithering method for the quantization.
   */
  inline Dithering dithering() const;

  /**
   * @brief Check whether quantization is enabled.
   */
  inline operator bool() const;

  /**
   * @brief Set the quantization level.
   */
  inline Quantization& level(Param level);

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
  Param m_level;

  /**
   * @brief The quantization dithering method.
   */
  Dithering m_dithering;

  // FIXME handle dither offset and seed
};

/**
 * @ingroup image_compression
 * @brief Interface for compression algorithms.
 */
class Algo {

  friend class Euclid::Fits::MefFile; // TODO rm if/when possible

public:
  explicit Algo() = default;
  ELEFITS_VIRTUAL_DTOR(Algo)
  ELEFITS_COPYABLE(Algo)
  ELEFITS_MOVABLE(Algo)

protected:
  virtual void compress(void* fptr) const = 0;
};

/**
 * @ingroup image_compression
 * @brief Intermediate class holding the tiling shape and quantization parameters.
 * 
 * Tiling shape is represented as a `Position<N>`.
 * The maximum dimension possible is equal to 6 (which is an internal CFITSIO limitation).
 * The value of `N` must therefore not exceed it.
 * 
 * @see rowwiseTiling()
 * @see maxTiling()
 */
template <typename TDerived> // FIXME rm N?
class AlgoMixin : public Algo {

public:
  ELEFITS_VIRTUAL_DTOR(AlgoMixin)
  ELEFITS_COPYABLE(AlgoMixin)
  ELEFITS_MOVABLE(AlgoMixin)

  /**
   * @brief Get the tiling.
   */
  const Position<-1>& shape() const;

  /**
   * @brief Get the quantization.
   */
  const Quantization& quantization() const; // FIXME move to children

  /**
   * @brief Set the tiling.
   */
  TDerived& shape(Position<-1> shape);

  /**
   * @brief Set the quantization.
   */
  TDerived& quantization(Quantization quantization);
  // FIXME HCompress does not support NonZeroPixel dithering
  // and therefore this method must be moved to the child classes
  // with specific check in HCompress

protected:
  /**
   * @brief Constructor.
   */
  explicit AlgoMixin(Position<-1> shape);

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
  Position<-1> m_shape;

  /**
   * @brief The quantization parameters.
   */
  Quantization m_quantization;
};

/**
 * @ingroup image_compression
 * @brief No compression.
 */
class None : public AlgoMixin<None> {

public:
  ELEFITS_VIRTUAL_DTOR(None)
  ELEFITS_COPYABLE(None)
  ELEFITS_MOVABLE(None)

  /**
   * @brief Constructor.
   */
  inline explicit None();
};

/**
 * @ingroup image_compression
 * @brief The Rice algorithm.
 */
class Rice : public AlgoMixin<Rice> {

public:
  ELEFITS_VIRTUAL_DTOR(Rice)
  ELEFITS_COPYABLE(Rice)
  ELEFITS_MOVABLE(Rice)

  /**
   * @brief Constructor.
   */
  inline explicit Rice(Position<-1> shape = rowwiseTiling());
};

/**
 * @ingroup image_compression
 * @brief The HCompress algorithm.
 */
class HCompress : public AlgoMixin<HCompress> {

public:
  ELEFITS_VIRTUAL_DTOR(HCompress)
  ELEFITS_COPYABLE(HCompress)
  ELEFITS_MOVABLE(HCompress)
  /**
   * @brief Constructor.
   */
  inline explicit HCompress(Position<-1> shape = rowwiseTiling(16));

  /**
   * @brief Get the scaling parameter.
   */
  inline const Param& scale() const;

  /**
   * @brief Check whether the image is smoothed at reading.
   */
  inline bool isSmooth() const;

  /**
   * @brief Set the scaling parameter.
   */
  inline HCompress& scale(Param scale);

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
   * @brief The scale parameter.
   */
  Param m_scale;

  /**
   * @brief The smoothing flag.
   */
  bool m_smooth;
};

/**
 * @ingroup image_compression
 * @brief The PLIO algorithm.
 * 
 * @warning Only integer values between 0 and 2^24 are supported.
 */
class Plio : public AlgoMixin<Plio> {

public:
  ELEFITS_VIRTUAL_DTOR(Plio)
  ELEFITS_COPYABLE(Plio)
  ELEFITS_MOVABLE(Plio)

  /**
   * @brief Constructor
   */
  inline explicit Plio(Position<-1> shape = rowwiseTiling());
};

/**
 * @ingroup image_compression
 * @brief The GZIP algorithm.
 */
class Gzip : public AlgoMixin<Gzip> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  /**
   * @brief Constructor
   */
  inline explicit Gzip(Position<-1> shape = rowwiseTiling());
};

/**
 * @ingroup image_compression
 * @brief The GZIP algorithm applied to "shuffled" pixel values.
 * 
 * Suffling means that value bytes are reordered such that most significant bytes of each value appear first.
 * Generally, this algorithm is much more efficient in terms of compression factor than GZIP, although it is a bit slower.
 */
class ShuffledGzip : public AlgoMixin<ShuffledGzip> { // FIXME merge with Gzip with option to shuffle

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  /**
   * @brief Constructor.
   */
  inline explicit ShuffledGzip(Position<-1> shape = rowwiseTiling());
};

/**
 * @brief Create a lossless algorithm well suited to the HDU properties.
 * @param bitpix The uncompressed data BITPIX
 * @param dimension The uncompressed data NAXIS
 */
inline std::unique_ptr<Algo> makeLosslessAlgo(long bitpix, long dimension) {
  std::unique_ptr<Algo> out;
  if (bitpix > 0 && bitpix <= 24) {
    out.reset(new Plio());
  } else if (dimension >= 2) {
    out.reset(new HCompress());
  } else {
    out.reset(new Rice());
  }
  return out;
}

/**
 * @brief Create a possibly lossy algorithm well suited to the HDU properties.
 * @param bitpix The uncompressed data BITPIX
 * @param dimension The uncompressed data NAXIS
 */
inline std::unique_ptr<Algo> makeAlgo(long bitpix, long dimension) {
  std::unique_ptr<Algo> out;
  const auto q4 = Quantization(Param::relative(4));
  if (bitpix > 0 && bitpix <= 24) {
    out.reset(new Plio());
  } else if (dimension >= 2) {
    out.reset(&(new HCompress())->quantization(std::move(q4)).scale(Param::relative(2.5)));
  } else {
    out.reset(&(new Rice())->quantization(std::move(q4)));
  }
  return out;
}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define COMPRESSION_IMPL
#include "EleFitsData/impl/Compression.hpp"
#undef COMPRESSION_IMPL
/// @endcond

#endif