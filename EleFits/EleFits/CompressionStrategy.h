// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_COMPRESSIONSTRATEGY_H
#define _ELEFITS_COMPRESSIONSTRATEGY_H

#include "EleFits/ImageHdu.h"
#include "EleFitsData/Compression.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_compression
 * @brief The interface for implementing compression strategies.
 */
struct CompressionStrategy {
#define ELEFITS_DECLARE_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  virtual const Compression& visit(const ImageHdu::Initializer<T>& init) = 0;
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_DECLARE_VISIT)
#undef ELEFITS_DECLARE_VISIT
};

/**
 * @ingroup image_compression
 * @brief A mixin to simplify `CompressionStrategy` implementation.
 * 
 * This class is a visitor-like mixin to mimic virtual method template.
 * Instead of overloading each call operator of the interface, only one template method is needed.
 */
template <typename TDerived>
struct CompressionStrategyMixin : public CompressionStrategy {
#define ELEFITS_IMPLEMENT_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  const Compression& visit(const ImageHdu::Initializer<T>& init) override { \
    return static_cast<TDerived&>(*this)(init); \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_IMPLEMENT_VISIT)
#undef ELEFITS_IMPLEMENT_VISIT
};

/**
 * @ingroup image_compression
 * @brief Strategy to set constantly the same algorithm unless incompatible with the HDU.
 * @tparam TAlgo The default compression algorithm
 * @tparam TFallback The fallback compression algorithm
 * 
 * For each HDU, if the default algorithm is compatible, it is returned.
 * Otherwise, the fallback algorithm is returned.
 * The latter must never fail, typically be a GZIP variant or `NoCompression`.
 * 
 * \par_example
 * \code
 * f.strategy(FallbackCompressionStrategy<Rice>::make());
 * \endcode
 */
template <typename TAlgo, typename TFallback = ShuffledGzip>
class FallbackCompressionStrategy : public CompressionStrategyMixin<FallbackCompressionStrategy<TAlgo, TFallback>> {

public:
  /**
   * @brief Create a strategy with a fallback derived from the default.
   * 
   * The fallback algorithm takes its tiling and quantization from the default one.
   */
  FallbackCompressionStrategy(TAlgo algo = TAlgo()) :
      m_algo(std::move(algo)), m_fallback(m_algo.tiling(), m_algo.quantization()) {}

  /**
   * @brief Create a strategy given explicit default and fallback algorithms.
   */
  FallbackCompressionStrategy(TAlgo algo, TFallback fallback) :
      m_algo(std::move(algo)), m_fallback(std::move(fallback)) {}

  /**
   * @brief Create the algorithm.
   */
  template <typename T>
  const Compression& operator()(const ImageHdu::Initializer<T>& init) {
    if (isCompatible(init)) {
      return m_algo;
    }
    return m_fallback;
  }

  /**
   * @brief Check whether the default algorithm is compatible with the HDU.
   */
  template <typename T>
  bool isCompatible(const ImageHdu::Initializer<T>& init) {

    // Universal algos: none or GZIP
    if constexpr (
        std::is_same_v<TAlgo, NoCompression> || std::is_same_v<TAlgo, Gzip> || std::is_same_v<TAlgo, ShuffledGzip>) {
      return true;
    }

    // Rice or H-compress
    if constexpr (std::is_same_v<TAlgo, Rice> || std::is_same_v<TAlgo, HCompress>) {
      return std::is_integral_v<T> || not m_algo.isLossless();
    }

    // PLIO
    if constexpr (std::is_same_v<TAlgo, Plio>) {

      // Floats
      if constexpr (bitpix<T>() < 0) {
        return false;
      }

      // Small enough ints
      if constexpr (bitpix<T>() < 24) {
        return true;
      }

      // Unable to check
      if (not init.data) {
        return false;
      }

      // Check max
      const auto max = *std::max_element(init.data, init.data + shapeSize(init.shape));
      return max < (std::size_t(1) << 24);
    }

    // Unknown algo
    return false;
  }

private:
  /**
   * @brief The default.
   */
  TAlgo m_algo;

  /**
   * @brief The fallback.
   */
  TFallback m_fallback;
};

/**
 * @ingroup image_compression
 * @brief A basic compression strategy.
 * 
 * This strategy selects the most appropriate compression algorithm at extension creation
 * according to simple criteria such as the pixel type and raster shape,
 * as well as the requested losslessness of the compression.
 * 
 * Small images (less than a FITS block size) are not compressed.
 * Otherwise, try the following algorithms in this order: `Plio`, `HCompress`, `Rice`.
 * If none of them is suitable (e.g. because lossless compression was requested even for floats),
 * then the `ShuffledGzip` is returned.
 * 
 * Makers `lossless()`, `losslessInt()` and `lossy()` simplify interfaces with `MefFile`
 * by creating the required `std::unique_ptr`:
 * 
 * \code
 * f.strategy(BasicCompressionStrategy::losslessInt());
 * \endcode
 */
class BasicCompressionStrategy : public CompressionStrategyMixin<BasicCompressionStrategy> {

private:
  /**
   * @brief Compression type.
   */
  enum class Type {
    Lossless, ///< Always lossless
    LosslessInt, ///< Lossless for integers, possibly lossy otherwise
    Lossy ///< Always possibly lossy
  };

  /**
   * @brief Constructor.
   */
  BasicCompressionStrategy(Type type) : m_type(type), m_algo() {}

public:
  /**
   * @brief Create a lossless strategy.
   */
  static BasicCompressionStrategy lossless() {
    return BasicCompressionStrategy(Type::Lossless);
  }

  /**
   * @brief Create a strategy which is lossless for integers and possibly lossy for floating point numbers.
   */
  static BasicCompressionStrategy losslessInt() {
    return BasicCompressionStrategy(Type::LosslessInt);
  }

  /**
   * @brief Create a possibly lossy strategy.
   */
  static BasicCompressionStrategy lossy() {
    return BasicCompressionStrategy(Type::Lossy);
  }

  /**
   * @brief Create the compression algorithm.
   */
  template <typename T>
  const Compression& operator()(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `ShuffledGzip` algorithm if compatible.
   */
  template <typename T>
  const std::unique_ptr<Compression>& gzip(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Rice` algorithm if compatible.
   */
  template <typename T>
  const std::unique_ptr<Compression>& rice(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `HCompress` algorithm if compatible.
   */
  template <typename T>
  const std::unique_ptr<Compression>& hcompress(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Plio` algorithm if compatible.
   */
  template <typename T>
  const std::unique_ptr<Compression>& plio(const ImageHdu::Initializer<T>& init);

private:
  /**
   * @brief Adapt the quantization to the pixel type and strategy type.
   */
  template <typename T>
  Compression::Quantization quantization() const;

  /**
   * @brief Adapt the tiling to the raster shape.
   */
  template <typename T>
  Position<-1> tiling(const ImageHdu::Initializer<T>& init) const;

  /**
   * @brief Adapt the H-compress tiling to the raster shape.
   */
  template <typename T>
  Position<-1> hcompressTiling(const ImageHdu::Initializer<T>& init) const;

  /**
   * @brief Adapt the H-compress scaling to the raster shape.
   */
  template <typename T>
  Compression::Scaling hcompressScaling() const;

  /**
   * @brief The compression type.
   */
  Type m_type;

  /**
   * @brief The algorithm cache.
   */
  std::unique_ptr<Compression> m_algo;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_COMPRESSIONSTRATEGY_IMPL
#include "EleFits/impl/CompressionStrategy.hpp"
#undef _ELEFITS_COMPRESSIONSTRATEGY_IMPL
/// @endcond

#endif
