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

/// @cond
// Forward declaration for friendship in Compression
class MefFile;
/// @endcond

/**
 * @brief Helper class for tile-related parameters.
 */
struct Tile {
  /**
   * @brief Scaling relative to the tile RMS.
   * 
   * \par_example
   * \code
   * auto disabled = Scaling(0);
   * auto absolute = Scaling(2);
   * auto relative1 = Tile::rms;
   * auto relative4 = Tile::rms / 4;
   * \endcode
   */
  static Scaling rms;

  /**
   * @brief Create a rowwise tiling.
   * @param rowCount The number of rows per tile
   */
  inline static Position<-1> rowwise(long rowCount = 1) {
    return Position<-1> {-1, rowCount};
  }

  /**
   * @brief Create a whole-data array tiling.
   */
  inline static Position<-1> whole() {
    return Position<-1> {-1};
  }
};

/**
 * @ingroup image_compression
 * @brief Interface for compression algorithms.
 * 
 * Tiling shape is represented as a `Position<-1>`.
 * The maximum dimension possible is equal to 6 (which is an internal CFITSIO limitation).
 * 
 * @see Tile::rowwise()
 * @see Tile::whole()
 */
class Compression {

  friend class Euclid::Fits::MefFile; // TODO rm if/when possible

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
  inline virtual bool isLossless() const;

protected:
  /**
   * @brief Constructor.
   */
  inline explicit Compression(Position<-1> tiling, Quantization quantization);

  /**
   * @brief Enable compression by CFITSIO.
   */
  virtual void compress(void* fptr) const = 0;

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
 * @ingroup image_compression
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

  /**
   * @brief Dependency inversion to call the wrapper's dispatch based on `TDerived`.
   */
  void compress(void* fptr) const final;
  // FIXME define the function here instead of in the wrapper
  // The function is not used anyway, and this simplifies compilation
  // => Change compress(fitsfile*, TDerived) into compress(void*, TDerived)
  // or forward declare fitsfile
};

/**
 * @ingroup image_compression
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
 * @ingroup image_compression
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
 * @ingroup image_compression
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
  inline explicit Rice(Position<-1> tiling = Tile::rowwise(), Quantization quantization = Quantization());
};

/**
 * @ingroup image_compression
 * @brief The H-compress algorithm.
 * 
 * This algorithm relies on some scaling parameter.
 * When scaling is enabled, H-compress is lossy irrespective of quantization.
 * In order to use H-compress losslessly, quantization and scaling must be zeroed,
 * and the data values must be integers.
 * 
 * Analogously to quantization, the scaling can be set globally or tile-wise.
 * In this case, it is generally provided as a multiplicative factor instead of an inverse:
 * 
 * \code
 * HCompress algo;
 * HCompress().scaling(Compression::rms * 2.5);
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
  inline bool isLossless() const override;

  /**
   * @brief Get the scaling parameter.
   */
  inline const Scaling& scaling() const;

  /**
   * @brief Check whether the image is smoothed at reading.
   */
  inline bool isSmooth() const;

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
  inline HCompress& enableSmoothing();

  /**
   * @brief Disable image smoothing at reading.
   */
  inline HCompress& disableSmoothing();

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
 * @ingroup image_compression
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