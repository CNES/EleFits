// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_COMPRESSIONSTRATEGY_H
#define _ELEFITS_COMPRESSIONSTRATEGY_H

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleFits/ImageHdu.h"
#include "EleFitsData/Compression.h"

namespace Euclid {
namespace Fits {

/**
 * @ingroup compression
 * @brief The interface for implementing compression actions.
 * 
 * To create a custom action, better inherit `CompressionActionMixin`, which simplifies implementation.
 * In this case, the only method which must be provided is:
 * 
 * \code
 * template <typename T>
 * bool apply(fitsfile*, const ImageHdu::Initializer<T>&);
 * \endcode
 * 
 * Internally, it must call `Cfitsio::ImageCompression::compress(fitsfile*, const Compression&)`
 * (maybe through `Compress::apply(fitsfile*, const Compression&)` and return `true` whenever possible.
 * If the action is not compatible with the initializer, then it must simply return `false`.
 */
class CompressionAction {
public:
  CompressionAction() = default;
  ELEFITS_COPYABLE(CompressionAction)
  ELEFITS_MOVABLE(CompressionAction)
  ELEFITS_VIRTUAL_DTOR(CompressionAction)

#define ELEFITS_DECLARE_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  virtual bool operator()(fitsfile* fptr, const ImageHdu::Initializer<T>& init) = 0;
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_DECLARE_VISIT)
#undef ELEFITS_DECLARE_VISIT
};

/**
 * @ingroup compression
 * @brief A mixin to simplify `CompressionAction` implementation.
 * 
 * This class is a visitor-like mixin to mimic virtual method template.
 * Instead of overloading each call operator of the interface, only one template method is needed.
 */
template <typename TDerived>
class CompressionActionMixin : public CompressionAction {
public:
  CompressionActionMixin() = default;
  ELEFITS_COPYABLE(CompressionActionMixin)
  ELEFITS_MOVABLE(CompressionActionMixin)
  ELEFITS_VIRTUAL_DTOR(CompressionActionMixin)

#define ELEFITS_IMPLEMENT_VISIT(T, name) \
  /** @brief Compress if the strategy is compatible with the initializer. */ \
  bool operator()(fitsfile* fptr, const ImageHdu::Initializer<T>& init) override { \
    return static_cast<TDerived&>(*this).apply(fptr, init); \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_IMPLEMENT_VISIT)
#undef ELEFITS_IMPLEMENT_VISIT
};

/**
 * @ingroup compression
 * @brief A compression action made of a single algorithm.
 * @tparam TAlgo The compression algorithm
 */
template <typename TAlgo>
class Compress : public CompressionActionMixin<Compress<TAlgo>> {
public:
  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit Compress(Ts&&... args) : m_algo(std::forward<Ts>(args)...) {}

  ELEFITS_VIRTUAL_DTOR(Compress)
  ELEFITS_COPYABLE(Compress)
  ELEFITS_MOVABLE(Compress)

  /**
   * @brief Try compressing.
   * 
   * If the algorithm is not compatible with the initializer,
   * then no compression is performed and `false` is returned.
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {

    // Compress if possible
    if (auto algo = compression(init)) {
      Cfitsio::ImageCompression::compress(fptr, *algo);
      return true;
    }

    return false;
  }

  /**
   * @brief Try creating a compression algorithm.
   * 
   * If the algorithm is not compatible with the initializer,
   * then `nullptr` is returned.
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init) {

    // CFITSIO does not support 64-bit integer compression
    if constexpr (bitpix<T>() == 64) {
      return nullptr;
    }

    // No compression of data units less than one block long
    static constexpr std::size_t block_size = 2880;
    if (shapeSize(init.shape) * sizeof(T) <= block_size) {
      return nullptr;
    }

    if (not can_compress(m_algo, init)) {
      return nullptr;
    }

    return std::make_unique<TAlgo>(m_algo);
  }

private:
  /**
   * @brief The algorithm.
   */
  TAlgo m_algo;
};

/**
 * @ingroup compression
 * @brief A restriction of `Compress` to integral values.
 * @see Compress
 */
template <typename TAlgo>
class CompressInts : public CompressionActionMixin<CompressInts<TAlgo>> {
public:
  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit CompressInts(Ts&&... args) : m_compress(std::forward<Ts>(args)...) {}

  ELEFITS_COPYABLE(CompressInts)
  ELEFITS_MOVABLE(CompressInts)
  ELEFITS_VIRTUAL_DTOR(CompressInts)

  /**
   * @copydoc Compress::apply()
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {
    if constexpr (not std::is_integral_v<T>) {
      return false;
    }
    return m_compress.apply(fptr, init);
  }

  /**
   * @copydoc Compress::compression()
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init) {
    if constexpr (not std::is_integral_v<T>) {
      return nullptr;
    }
    return m_compress.compression(init);
  }

private:
  /**
   * @brief The parent compression action.
   */
  Compress<TAlgo> m_compress;
};

/**
 * @ingroup compression
 * @brief A restriction of `Compress` to floating point values.
 * @see Compress
 */
template <typename TAlgo>
class CompressFloats : public CompressionActionMixin<CompressFloats<TAlgo>> {
public:
  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit CompressFloats(Ts&&... args) : m_compress(std::forward<Ts>(args)...) {}

  ELEFITS_COPYABLE(CompressFloats)
  ELEFITS_MOVABLE(CompressFloats)
  ELEFITS_VIRTUAL_DTOR(CompressFloats)

  /**
   * @copydoc Compress::apply()
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init) {
    if constexpr (not std::is_floating_point_v<T>) {
      return false;
    }
    return m_compress.apply(fptr, init);
  }

  /**
   * @copydoc Compress::compression()
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init) {
    if constexpr (not std::is_floating_point_v<T>) {
      return nullptr;
    }
    return m_compress.compression(init);
  }

private:
  /**
   * @brief The parent compression action.
   */
  Compress<TAlgo> m_compress;
};

/// @cond
template <typename TAlgo, typename T>
bool can_compress(const TAlgo&, const ImageHdu::Initializer<T>&) {
  return true;
}

template <typename T>
bool can_compress(const Rice& algo, const ImageHdu::Initializer<T>&) {
  return std::is_integral_v<T> || not algo.is_lossless();
}

template <typename T>
bool can_compress(const HCompress& algo, const ImageHdu::Initializer<T>& init) {
  const auto& shape = init.shape;
  if (shapeSize(shape) < 2 || shape[0] < 4 || shape[1] < 4) {
    return false;
  }
  return std::is_integral_v<T> || not algo.is_lossless();
}

template <typename T>
bool can_compress(const Plio&, const ImageHdu::Initializer<T>& init) {

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
 * @ingroup compression
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
 */
class CompressAuto : public CompressionActionMixin<CompressAuto> {

public:
  /**
   * @brief Constructor.
   */
  explicit CompressAuto(CompressionType type = CompressionType::Lossless) : m_type(type) {}

  /**
   * @brief Compress if possible.
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `ShuffledGzip` action if compatible.
   */
  template <typename T>
  std::unique_ptr<Compress<ShuffledGzip>> gzip(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Rice` action if compatible.
   */
  template <typename T>
  std::unique_ptr<Compress<Rice>> rice(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `HCompress` action if compatible.
   */
  template <typename T>
  std::unique_ptr<Compress<HCompress>> hcompress(const ImageHdu::Initializer<T>& init);

  /**
   * @brief Create a `Plio` action if compatible.
   */
  template <typename T>
  std::unique_ptr<Compress<Plio>> plio(const ImageHdu::Initializer<T>& init);

private:
  /**
   * @brief Adapt the quantization to the pixel type and strategy type.
   */
  template <typename T>
  Quantization quantization() const;

  /**
   * @brief Adapt the tiling to the raster shape.
   */
  template <typename T>
  Position<-1> tiling(const ImageHdu::Initializer<T>& init) const;

  /**
   * @brief Adapt the H-compress tiling to the raster shape.
   */
  template <typename T>
  Position<-1> hcompress_tiling(const ImageHdu::Initializer<T>& init) const;

  /**
   * @brief Adapt the H-compress scaling to the raster shape.
   */
  template <typename T>
  Scaling hcompress_scaling() const;

  /**
   * @brief The compression type.
   */
  CompressionType m_type;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_COMPRESSIONSTRATEGY_IMPL
#include "EleFits/impl/CompressionStrategy.hpp"
#undef _ELEFITS_COMPRESSIONSTRATEGY_IMPL
/// @endcond

#endif
