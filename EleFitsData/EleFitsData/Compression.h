// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Position.h"
#include "EleFitsData/Raster.h"

#include <memory>
#include <string>

namespace Euclid {
namespace Fits {

/// @cond
// Forward declaration for friendship in Compression
class MefFile;
/// @endcond

/**
 * @ingroup image_compression
 * @brief Interface for compression algorithms.
 * 
 * Tiling shape is represented as a `Position<-1>`.
 * The maximum dimension possible is equal to 6 (which is an internal CFITSIO limitation).
 * 
 * @see rowwiseTiling()
 * @see maxTiling()
 */
class Compression {

  friend class Euclid::Fits::MefFile; // TODO rm if/when possible

public:
  explicit Compression() = default;
  ELEFITS_VIRTUAL_DTOR(Compression)
  ELEFITS_COPYABLE(Compression)
  ELEFITS_MOVABLE(Compression)

  /**
   * @brief Helper class for scaling parameters.
   * 
   * A scaling can be disabled, or provided as an absolute value,
   * or provided as a tile noise RMS-relative value.
   * 
   * Instance `Compression::rms` eases the making of such parameters:
   * 
   * \code
   * auto disabledScaling = 0;
   * auto absoluteScaling = 4;
   * auto relativeScaling = Compression::rms / 4;
   * \endcode
  */
  class Scaling {
  public:
    /**
     * @brief The type of scaling value.
     */
    enum class Type {
      Absolute, ///< Absolute
      Factor, ///< Relative as factor: absolute = RMS * value
      Inverse ///< Relative as inverse: absolute = RMS / value
    };

    /**
     * @brief Constructor.
     */
    inline Scaling(double value, Type type = Type::Absolute);
    // Purposedly not explicit to support implicit cast from double

    /**
     * @brief Check whether the scaling is enabled (i.e. the value is not null).
     */
    inline operator bool() const;

    /**
     * @brief Get the scaling type.
     */
    inline Type type() const;

    /**
     * @brief Get the scaling value.
    */
    inline double value() const;

    /**
     * @brief Check whether two parameters are equal.
     */
    inline bool operator==(const Scaling& rhs) const;

    /**
     * @brief Check whether two parameters are different.
     */
    inline bool operator!=(const Scaling& rhs) const;

  private:
    /**
     * @brief The value type.
     */
    Type m_type;

    /**
     * @brief The scaling value.
     */
    double m_value;
  };

  /**
   * @brief Helper class for tile noise RMS-relative scaling.
   */
  struct TileRms {
    /**
     * @brief Create a scaling factor.
     */
    Compression::Scaling operator*(double f) const {
      return Scaling(f, Scaling::Type::Factor);
    }

    /**
     * @brief Create a scaling inverse factor.
     */
    Compression::Scaling operator/(double f) const {
      return Scaling(f, Scaling::Type::Inverse);
    }
  };

  /**
   * @brief Helper instance for tile noise RMS-relative scaling.
   */
  static TileRms rms;

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
    ELEFITS_VIRTUAL_DTOR(Quantization)
    ELEFITS_COPYABLE(Quantization)
    ELEFITS_MOVABLE(Quantization)

    /**
     * @brief Default, lossless compression constructor.
     */
    inline explicit Quantization();

    /**
     * @brief Level-based constructor.
     * 
     * The default dithering for lossy compression is `Dithering::EveryPixel`.
     */
    inline explicit Quantization(Compression::Scaling level);

    /**
     * @brief Full constructor.
     */
    inline explicit Quantization(Compression::Scaling level, Dithering method);

    /**
     * @brief Get the quantization level
     */
    inline const Compression::Scaling& level() const;

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
    inline Quantization& level(Compression::Scaling level);

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
    Compression::Scaling m_level;

    /**
     * @brief The quantization dithering method.
     */
    Dithering m_dithering;

    // FIXME handle dither offset and seed
  };

  /**
   * @brief Create a lossless algorithm well suited to the HDU properties.
   * @param bitpix The uncompressed data BITPIX
   * @param dimension The uncompressed data NAXIS
   */
  inline static std::unique_ptr<Compression> makeLosslessAlgo(long bitpix, long dimension);

  /**
   * @brief Create a lossless algorithm well suited to a given raster.
   */
  template <typename TRaster>
  static std::unique_ptr<Compression> makeLosslessAlgo(const TRaster& raster);

  /**
   * @brief Create a possibly lossy algorithm well suited to a given raster.
   */
  template <typename TRaster>
  static std::unique_ptr<Compression> makeAlgo(const TRaster& raster);

  /**
   * @brief Create a possibly lossy algorithm well suited to the HDU properties.
   * @param bitpix The uncompressed data BITPIX
   * @param dimension The uncompressed data NAXIS
   */
  inline static std::unique_ptr<Compression> makeAlgo(long bitpix, long dimension);

  /**
   * @brief Create a rowwise tiling.
   * @param rowCount The number of rows per tile
   */
  inline static Position<-1> rowwiseTiling(long rowCount = 1) {
    return Position<-1> {-1, rowCount};
  }

  /**
   * @brief Create a whole-data array tiling.
   */
  inline static Position<-1> maxTiling() {
    return Position<-1> {-1};
  }

  /**
   * @brief Get the tiling.
   */
  inline const Position<-1>& tiling() const;

  /**
   * @brief Get the quantization.
   */
  inline const Quantization& quantization() const;

  /**
   * @brief Check whether the compression is lossless.
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
 */
class Gzip : public AlgoMixin<Gzip> {

public:
  ELEFITS_VIRTUAL_DTOR(Gzip)
  ELEFITS_COPYABLE(Gzip)
  ELEFITS_MOVABLE(Gzip)

  /**
   * @brief Constructor
   */
  inline explicit Gzip(Position<-1> tiling = rowwiseTiling(), Quantization quantization = Quantization());
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
  inline explicit ShuffledGzip(Position<-1> tiling = rowwiseTiling(), Quantization quantization = Quantization());
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
  inline explicit Rice(Position<-1> tiling = rowwiseTiling(), Quantization quantization = Quantization());
};

/**
 * @ingroup image_compression
 * @brief The H-compress algorithm.
 */
class HCompress : public AlgoMixin<HCompress> {

public:
  ELEFITS_VIRTUAL_DTOR(HCompress)
  ELEFITS_COPYABLE(HCompress)
  ELEFITS_MOVABLE(HCompress)
  /**
   * @brief Constructor.
   */
  inline explicit HCompress(Position<-1> tiling = rowwiseTiling(16), Quantization quantization = Quantization());

  using Compression::quantization;

  /**
   * @brief Check whether compression is lossless.
   */
  inline bool isLossless() const override;

  /**
   * @brief Get the scaling parameter.
   */
  inline const Compression::Scaling& scaling() const;

  /**
   * @brief Check whether the image is smoothed at reading.
   */
  inline bool isSmooth() const;

  /**
   * @brief Set the quantization.
   * 
   * H-compress does not support `Dithering::NonZeroPixel`.
   */
  inline HCompress& quantization(Compression::Quantization quantization) override;

  /**
   * @brief Set the scaling parameter.
   */
  inline HCompress& scaling(Compression::Scaling scale);

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
  Compression::Scaling m_scale;

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
  inline explicit Plio(Position<-1> tiling = rowwiseTiling(), Quantization quantization = Quantization());
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define COMPRESSION_IMPL
#include "EleFitsData/impl/Compression.hpp"
#undef COMPRESSION_IMPL
/// @endcond

#endif