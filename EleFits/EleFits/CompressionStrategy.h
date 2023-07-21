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
 * @brief A compression strategy with a chain of algorithms.
 * @tparam TPrime Primary compression algorithm
 * @tparam TFallbacks Ordered list of fallback compression algorithms
 * 
 * If the primary algorithm is not suitable, then the first fallback algorithm is tested, then the second one, and so on.
 * If the last fallback algorithm is not suitable, then no compression is applied.
 */
template <typename TPrime, typename... TFallbacks>
class Compress : public CompressionStrategyMixin<Compress<TPrime, TFallbacks...>> {
public:
  /**
   * @brief Create a strategy with default-initialized fallbacks.
   */
  explicit Compress(TPrime prime = TPrime()) : m_prime(std::move(prime)), m_fallbacks(TFallbacks()...) {}

  /**
   * @brief Create a strategy with explicit parameters.
   * @param prime The primary algorithm
   * @param fallbacks The fallback algorithms
   * 
   * If not all fallback algorithms are specified, then the last ones are default-initialized.
   */
  template <typename... Ts>
  explicit Compress(TPrime prime, Ts&&... fallbacks) :
      m_prime(std::move(prime)), m_fallbacks(std::forward<Ts>(fallbacks)...) {}

  /**
   * @brief Create the compression algorithm.
   */
  template <typename T>
  const Compression& operator()(const ImageHdu::Initializer<T>& init) {
    if (shouldCompress(m_prime, init)) {
      if (m_prime.tiling() == Compression::rowwiseTiling()) {
        const long rowSize = init.shape[0] * sizeof(T);
        static constexpr long minTileSize = 1024 * 1024;
        const long rowCount = minTileSize / rowSize + 1;
        m_prime.tiling(Compression::rowwiseTiling(rowCount));
      }
      return m_prime;
    }
    return m_fallbacks(init);
  }

private:
  /**
   * @brief The prime.
   */
  TPrime m_prime;

  /**
   * @brief The fallbacks.
   */
  Compress<TFallbacks...> m_fallbacks;
};

/// @cond
/**
 * @brief Single-algorithm specialization.
 */
template <typename TAlgo>
class Compress<TAlgo> : public CompressionStrategyMixin<Compress<TAlgo>> {
public:
  /**
   * @brief Constructor.
   */
  Compress(TAlgo algo = TAlgo()) : m_prime(std::move(algo)) {}

  /**
   * @brief Create the compression algorithm.
   */
  template <typename T>
  const Compression& operator()(const ImageHdu::Initializer<T>& init) {
    if (m_prime.tiling() == Compression::rowwiseTiling()) { // FIXME duplicates
      const long rowWidth = init.shape[0];
      const long rowSize = rowWidth * sizeof(T);
      static constexpr long minTileSize = 1024 * 1024;
      const long rowCount = minTileSize / rowSize + 1;
      printf("%li x %li\n", rowWidth, rowCount);
      m_prime.tiling({rowWidth, rowCount});
    }
    if (not shouldCompress(m_prime, init)) {
      static NoCompression none;
      return none;
    }
    return m_prime;
  }

private:
  TAlgo m_prime;
};

template <typename TAlgo, typename T>
bool shouldCompress(const TAlgo& algo, const ImageHdu::Initializer<T>& init) {
  if constexpr (std::is_same_v<TAlgo, NoCompression>) {
    return true;
  }
  static constexpr std::size_t blockSize = 2880;
  if (shapeSize(init.shape) * sizeof(T) <= blockSize) {
    return false;
  }
  if constexpr (bitpix<T>() == 64) { // CFITSIO does not support 64-bit integer compression
    return false;
  }
  return canCompress(algo, init);
}

template <typename TAlgo, typename T>
bool canCompress(const TAlgo&, const ImageHdu::Initializer<T>&) {
  return true;
}

template <typename T>
bool canCompress(const Rice& algo, const ImageHdu::Initializer<T>&) {
  return std::is_integral_v<T> || not algo.isLossless();
}

template <typename T>
bool canCompress(const HCompress& algo, const ImageHdu::Initializer<T>& init) {
  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return false;
  }
  return std::is_integral_v<T> || not algo.isLossless();
}

template <typename T>
bool canCompress(const Plio&, const ImageHdu::Initializer<T>& init) {

  constexpr auto bp = bitpix<T>();

  // Float or too large int
  if constexpr (bp < 0 || bp > 32) {
    return false;
  }

  // Maybe
  if constexpr (bp > 16) {
    if (not init.data) {
      return false;
    }
    const auto max = *std::max_element(init.data, init.data + shapeSize(init.shape));
    if (max >= (T(1) << 24)) {
      return false;
    }
  }

  return true;
}
/// @endcond

/**
 * @ingroup image_compression
 * @brief A basic adaptive compression strategy.
 * 
 * This strategy selects the most appropriate compression algorithm at extension creation
 * according to simple criteria such as the pixel type and raster shape,
 * as well as the requested losslessness of the compression.
 * 
 * Small images (less than a FITS block size) are not compressed.
 * Otherwise, the following algorithms are tried in this order: `Plio`, `HCompress`, `Rice`.
 * If none of them is suitable (e.g. because lossless compression was requested even for floats),
 * then the `ShuffledGzip` is returned.
 * 
 * Strategies are created with makers `lossless()`, `losslessInt()` and `lossy()`, e.g.:
 * 
 * \code
 * f.strategy(CompressAptly::losslessInt());
 * \endcode
 */
class CompressAptly : public CompressionStrategyMixin<CompressAptly> {

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
  CompressAptly(Type type) : m_type(type), m_algo() {}

public:
  /**
   * @brief Create a lossless strategy.
   */
  static CompressAptly lossless() {
    return CompressAptly(Type::Lossless);
  }

  /**
   * @brief Create a strategy which is lossless for integers and possibly lossy for floating point numbers.
   */
  static CompressAptly losslessInt() {
    return CompressAptly(Type::LosslessInt);
  }

  /**
   * @brief Create a possibly lossy strategy.
   */
  static CompressAptly lossy() {
    return CompressAptly(Type::Lossy);
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
