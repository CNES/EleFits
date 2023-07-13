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
 * @brief The interface for implementing compression strategies.
 */
struct CompressionStrategy {
#define ELEFITS_DECLARE_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  virtual std::unique_ptr<Compression> visit(const ImageHdu::Initializer<T>& init) = 0;
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_DECLARE_VISIT)
#undef ELEFITS_DECLARE_VISIT
};

/**
 * @brief A mixin to simplify `CompressionStrategy` implementation.
 * 
 * This class is a visitor-like mixin to mimic virtual method template.
 * Instead of overloading each call operator of the interface, only one template method is needed.
 */
template <typename TDerived>
struct CompressionStrategyMixin : public CompressionStrategy {
#define ELEFITS_IMPLEMENT_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  std::unique_ptr<Compression> visit(const ImageHdu::Initializer<T>& init) override { \
    return static_cast<TDerived&>(*this)(init); \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_IMPLEMENT_VISIT)
#undef ELEFITS_IMPLEMENT_VISIT
};

/**
 * @brief Strategy to set constantly the same algorithm unless incompatible with the HDU.
 * 
 * For each HDU, if the default algorithm is compatible, it is return.
 * Otherwise, the fallback algorithm is returned.
 */
template <typename TAlgo, typename TFallback = ShuffledGzip>
class FallbackCompressionStrategy : public CompressionStrategyMixin<FallbackCompressionStrategy<TAlgo, TFallback>> {

public:
  /**
   * @brief Create a strategy with a fallback derived from the default.
   * 
   * The fallback algorithm takes its tiling and quantization from the default one.
   */
  FallbackCompressionStrategy(TAlgo algo) :
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
  std::unique_ptr<Compression> operator()(const ImageHdu::Initializer<T>& init) {
    if (isCompatible(init)) {
      return std::make_unique<TAlgo>(m_algo);
    }
    return std::make_unique<TFallback>(m_fallback);
  }

  /**
   * @brief Check whether the default algorithm is compatible with the HDU.
   */
  template <typename T>
  bool isCompatible(const ImageHdu::Initializer<T>& init) {
    // FIXME
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
 */
class BasicCompressionStrategy : public CompressionStrategyMixin<BasicCompressionStrategy> {

private:
  enum class Type {
    Lossless,
    LosslessInt,
    Lossy
  };

  BasicCompressionStrategy(Type type) : m_type(type) {}

public:
  /**
   * @brief Create a lossless strategy.
   */
  static std::unique_ptr<BasicCompressionStrategy> lossless() {
    return std::unique_ptr<BasicCompressionStrategy>(new BasicCompressionStrategy(Type::Lossless));
  }

  /**
   * @brief Create a strategy which is lossless for integers and possibly lossy for floating point numbers.
   */
  static std::unique_ptr<BasicCompressionStrategy> losslessInt() {
    return std::unique_ptr<BasicCompressionStrategy>(new BasicCompressionStrategy(Type::LosslessInt));
  }

  /**
   * @brief Create a possibly lossy strategy.
   */
  static std::unique_ptr<BasicCompressionStrategy> lossy() {
    return std::unique_ptr<BasicCompressionStrategy>(new BasicCompressionStrategy(Type::Lossy));
  }

  /**
   * @brief Create the compression algorithm.
   */
  template <typename T>
  std::unique_ptr<Compression> operator()(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `ShuffledGzip` algorithm if compatible.
   */
  template <typename T>
  std::unique_ptr<ShuffledGzip> gzip(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Rice` algorithm if compatible.
   */
  template <typename T>
  std::unique_ptr<Rice> rice(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `HCompress` algorithm if compatible.
   */
  template <typename T>
  std::unique_ptr<HCompress> hcompress(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Plio` algorithm if compatible.
   */
  template <typename T>
  std::unique_ptr<Plio> plio(const ImageHdu::Initializer<T>& init);

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

  Type m_type;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_COMPRESSIONSTRATEGY_IMPL
#include "EleFits/impl/CompressionStrategy.hpp"
#undef _ELEFITS_COMPRESSIONSTRATEGY_IMPL
/// @endcond

#endif
