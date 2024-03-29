// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_COMPRESSION_H
#define _ELEFITSDATA_COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Quantization.h"
#include "EleFitsData/Raster.h"
#include "EleFitsData/Scaling.h"

#include <memory>
#include <string>

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
   * @brief Create an adaptive tiling.
   * 
   * The tile size will be adapted to the image data,
   * while ensuring that tile data is contiguous in memory.
   */
  inline static Linx::Position<-1> adaptive()
  {
    return Linx::Position<-1> {};
  }

  /**
   * @brief Create a rowwise tiling.
   * @param row_count The number of rows per tile
   */
  inline static Linx::Position<-1> rowwise(Linx::Index row_count = 1)
  {
    return Linx::Position<-1> {-1, row_count};
  }

  /**
   * @brief Create a whole-data array tiling.
   */
  inline static Linx::Position<-1> whole()
  {
    return Linx::Position<-1> {-1};
  }
};

/**
 * @ingroup compression
 * @brief Interface for compression algorithms.
 * 
 * Tiling shape is represented as a `Linx::Position<-1>`.
 * The maximum dimension possible is equal to 6 (which is an internal CFITSIO limitation).
 * 
 * @see Tile::rowwise()
 * @see Tile::whole()
 */
class Compression {
public:

  explicit Compression() = default;
  LINX_VIRTUAL_DTOR(Compression)
  LINX_DEFAULT_COPYABLE(Compression)
  LINX_DEFAULT_MOVABLE(Compression)

  /**
   * @brief Get the tiling.
   */
  inline const Linx::Position<-1>& tiling() const;

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
  inline explicit Compression(Linx::Position<-1> tiling, Quantization quantization);

  /**
   * @brief The tiling shape.
   */
  Linx::Position<-1> m_tiling;

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

  LINX_VIRTUAL_DTOR(AlgoMixin)
  LINX_DEFAULT_COPYABLE(AlgoMixin)
  LINX_DEFAULT_MOVABLE(AlgoMixin)

  using Compression::tiling;

  using Compression::quantization;

  /**
   * @brief Set the tiling.
   */
  virtual TDerived& tiling(Linx::Position<-1> shape);

  /**
   * @brief Set the quantization.
   */
  virtual TDerived& quantization(Quantization quantization);

protected:

  /**
   * @brief Constructor.
   */
  explicit AlgoMixin(Linx::Position<-1> tiling, Quantization quantization);
};

/**
 * @ingroup compression
 * @brief No compression.
 */
class NoCompression : public AlgoMixin<NoCompression> {
public:

  LINX_VIRTUAL_DTOR(NoCompression)
  LINX_DEFAULT_COPYABLE(NoCompression)
  LINX_DEFAULT_MOVABLE(NoCompression)

  /**
   * @brief Constructor.
   */
  inline explicit NoCompression();

  using Compression::tiling;

  using Compression::quantization;

  /**
   * @brief Disabled setter.
   */
  inline NoCompression& tiling(Linx::Position<-1>) override;

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

  LINX_VIRTUAL_DTOR(Gzip)
  LINX_DEFAULT_COPYABLE(Gzip)
  LINX_DEFAULT_MOVABLE(Gzip)

  /**
   * @brief Constructor
   */
  inline explicit Gzip(Linx::Position<-1> tiling = Tile::adaptive(), Quantization quantization = Quantization());
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

  LINX_VIRTUAL_DTOR(ShuffledGzip)
  LINX_DEFAULT_COPYABLE(ShuffledGzip)
  LINX_DEFAULT_MOVABLE(ShuffledGzip)

  /**
   * @brief Constructor.
   */
  inline explicit ShuffledGzip(
      Linx::Position<-1> tiling = Tile::adaptive(),
      Quantization quantization = Quantization());
};

/**
 * @ingroup compression
 * @brief The Rice algorithm.
 */
class Rice : public AlgoMixin<Rice> {
public:

  LINX_VIRTUAL_DTOR(Rice)
  LINX_DEFAULT_COPYABLE(Rice)
  LINX_DEFAULT_MOVABLE(Rice)

  /**
   * @brief Constructor.
   */
  inline explicit Rice(Linx::Position<-1> tiling = Tile::adaptive(), Quantization quantization = Quantization());
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

  LINX_VIRTUAL_DTOR(HCompress)
  LINX_DEFAULT_COPYABLE(HCompress)
  LINX_DEFAULT_MOVABLE(HCompress)
  /**
   * @brief Constructor.
   */
  inline explicit HCompress(
      Linx::Position<-1> tiling = Tile::adaptive(),
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
   * @brief Set the quantization.
   * 
   * H-compress does not support `Dithering::NonZeroPixel`.
   */
  inline HCompress& quantization(Quantization quantization) override;

  /**
   * @brief Set the scaling parameter.
   */
  inline HCompress& scaling(Scaling scale);

private:

  /**
   * @brief The scale parameter.
   */
  Scaling m_scale;
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

  LINX_VIRTUAL_DTOR(Plio)
  LINX_DEFAULT_COPYABLE(Plio)
  LINX_DEFAULT_MOVABLE(Plio)

  /**
   * @brief Constructor
   */
  inline explicit Plio(Linx::Position<-1> tiling = Tile::adaptive(), Quantization quantization = Quantization());
};

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_COMPRESSION_IMPL
#include "EleFitsData/impl/Compression.hpp"
#undef _ELEFITSDATA_COMPRESSION_IMPL
/// @endcond

#endif