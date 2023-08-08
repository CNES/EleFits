// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_COMPRESSION_H
#define _ELEFITSDATA_COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Position.h"
#include "EleFitsData/Quantization.h"
#include "EleFitsData/Raster.h"
#include "EleFitsData/Scaling.h"

#include <memory>
#include <string>

namespace Euclid {
namespace Fits {

/**
 * @brief Compression type.
 */
enum class CompressionType {
  Lossless, ///< Always lossless
  LosslessInts, ///< Lossless for integers, possibly lossy otherwise
  Lossy ///< Always possibly lossy
};

/**
 * @ingroup compression
 * @brief Helper class for tile-related parameters.
 */
struct Tile {
  /**
   * @brief Scaling relative to the tile RMS.
   * 
   * \par_example
   * \code
   * auto disabled = Quantization(0);
   * auto absolute = Quantization(100);
   * auto relative1 = Quantization(Tile::rms);
   * auto relative4 = Quantization(Tile::rms / 4);
   * \endcode
   */
  static Scaling rms;

  /**
   * @brief Create a rowwise tiling.
   * @param row_count The number of rows per tile
   */
  inline static Position<-1> rowwise(long row_count = 1) {
    return Position<-1> {-1, row_count};
  }

  /**
   * @brief Create a whole-data array tiling.
   */
  inline static Position<-1> whole() {
    return Position<-1> {-1};
  }
};

/**
 * @ingroup compression
 * @brief Interface for compression algorithms.
 * 
 * Tiling shape is represented as a `Position<-1>`.
 * The maximum dimension possible is equal to 6 (which is an internal CFITSIO limitation).
 * 
 * @see Tile::rowwise()
 * @see Tile::whole()
 */
class Compression {

public:
  explicit Compression() = default;
  ELEFITS_VIRTUAL_DTOR(Compression)
  ELEFITS_COPYABLE(Compression)
  ELEFITS_MOVABLE(Compression)

  /**
   * @brief Get the tiling.
   */
  inline const Position<-1>& tiling() const;

  /**
   * @brief Get the quantization.
   */
  inline const Quantization& quantization() const;

  /**
   * @brief Check whether the compression is lossless for.
   */
  inline virtual bool is_lossless() const;

protected:
  /**
   * @brief Constructor.
   */
  inline explicit Compression(Position<-1> tiling, Quantization quantization);

  /**
   * @brief The tiling shape.
   */
  Position<-1> m_tiling;

  /**
   * @brief The quantization parameters.
   */
  Quantization m_quantization;
};

/**
 * @ingroup compression
 * @brief Intermediate class for internal dispatching.
 */
template <typename TDerived>
class AlgoMixin : public Compression {

public:
  ELEFITS_VIRTUAL_DTOR(AlgoMixin)
  ELEFITS_COPYABLE(AlgoMixin)
  ELEFITS_MOVABLE(AlgoMixin)

  using Compression::tiling;

  using Compression::quantization;

  /**
   * @brief Set the tiling.
   */
  virtual TDerived& tiling(Position<-1> shape);

  /**
   * @brief Set the quantization.
   */
  virtual TDerived& quantization(Quantization quantization);

protected:
  /**
   * @brief Constructor.
   */
  explicit AlgoMixin(Position<-1> tiling, Quantization quantization);
};

/**
 * @ingroup compression
 * @brief No compression.
 */
class NoCompression : public AlgoMixin<NoCompression> {

public:
  ELEFITS_VIRTUAL_DTOR(NoCompression)
  ELEFITS_COPYABLE(NoCompression)
  ELEFITS_MOVABLE(NoCompression)

  /**
   * @brief Constructor.
   */
  inline explicit NoCompression();

  using Compression::tiling;

  using Compression::quantization;

  /**
   * @brief Disabled setter.
   */
  inline NoCompression& tiling(Position<-1>) override;

  /**
   * @brief Disabled setter.
   */
  inline NoCompression& quantization(Quantization) override;
};

/**
 * @ingroup compression
 * @brief The GZIP algorithm.
 * 
 * Along with `ShuffledGzip`, this is the only algorithm which supports lossless compression of floating point data.
 */
class Gzip : public AlgoMixin<Gzip> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  /**
   * @brief Constructor
   */
  inline explicit Gzip(Position<-1> tiling = Tile::rowwise(), Quantization quantization = Quantization());
};

/**
 * @ingroup compression
 * @brief The GZIP algorithm applied to "shuffled" pixel values.
 * 
 * Suffling means that value bytes are reordered such that most significant bytes of each value appear first.
 * Generally, this algorithm is more efficient in terms of compression factor than GZIP, although it is a bit slower.
 */
class ShuffledGzip : public AlgoMixin<ShuffledGzip> { // FIXME merge with Gzip with option to shuffle

public:
  ELEFITS_VIRTUAL_DTOR(ShuffledGzip)
  ELEFITS_COPYABLE(ShuffledGzip)
  ELEFITS_MOVABLE(ShuffledGzip)

  /**
   * @brief Constructor.
   */
  inline explicit ShuffledGzip(Position<-1> tiling = Tile::rowwise(), Quantization quantization = Quantization());
};

/**
 * @ingroup compression
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
  inline explicit Rice(Position<-1> tiling = Tile::rowwise(), Quantization quantization = Quantization());
};

/**
 * @ingroup compression
 * @brief The H-compress algorithm.
 * 
 * This algorithm relies on some scaling parameter.
 * When scaling is enabled, H-compress is lossy irrespective of quantization.
 * In order to use H-compress losslessly, quantization and scaling must be zeroed,
 * and the data values must be integers.
 * 
 * Analogously to quantization, the scaling can be set globally or tile-wise, which is recommended.
 * In the latter case, it is generally provided as a multiplicative factor of the noise RMS:
 * 
 * \code
 * HCompress algo;
 * algo.quantization(Tile::rms / 4);
 * algo.scaling(Tile::rms * 2.5);
 * \endcode
 */
class HCompress : public AlgoMixin<HCompress> {

public:
  ELEFITS_VIRTUAL_DTOR(HCompress)
  ELEFITS_COPYABLE(HCompress)
  ELEFITS_MOVABLE(HCompress)
  /**
   * @brief Constructor.
   */
  inline explicit HCompress(
      Position<-1> tiling = Tile::rowwise(16),
      Quantization quantization = Quantization(),
      Scaling scaling = Scaling(0));

  using Compression::quantization;

  /**
   * @brief Check whether compression is lossless.
   */
  inline bool is_lossless() const override;

  /**
   * @brief Get the scaling parameter.
   */
  inline const Scaling& scaling() const;

  /**
   * @brief Check whether the image is smoothed at reading.
   */
  inline bool is_smooth() const;

  /**
   * @brief Set the quantization.
   * 
   * H-compress does not support `Dithering::NonZeroPixel`.
   */
  inline HCompress& quantization(Quantization quantization) override;

  /**
   * @brief Set the scaling parameter.
   */
  inline HCompress& scaling(Scaling scale);

  /**
   * @brief Enable image smoothing at reading.
   */
  inline HCompress& enable_smoothing();

  /**
   * @brief Disable image smoothing at reading.
   */
  inline HCompress& disable_smoothing();

private:
  /**
   * @brief The scale parameter.
   */
  Scaling m_scale;

  /**
   * @brief The smoothing flag.
   */
  bool m_smooth;
};

/**
 * @ingroup compression
 * @brief The PLIO algorithm.
 * 
 * This algorithm was designed specifically for bitmasks,
 * and performs well for rasters with constant regions.
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
  inline explicit Plio(Position<-1> tiling = Tile::rowwise(), Quantization quantization = Quantization());
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_COMPRESSION_IMPL
#include "EleFitsData/impl/Compression.hpp"
#undef _ELEFITSDATA_COMPRESSION_IMPL
/// @endcond

#endif