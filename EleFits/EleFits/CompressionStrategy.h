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

/// @cond
template <long N>
Position<N> unravel_index(long index, Position<N> shape) // FIXME to Linx
{
  for (auto& s : shape) {
    const long length = s;
    s = index % length;
    index /= length;
  }
  return shape;
}
/// @endcond

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

  /// @group_construction

  CompressionAction() = default;
  ELEFITS_COPYABLE(CompressionAction)
  ELEFITS_MOVABLE(CompressionAction)
  ELEFITS_VIRTUAL_DTOR(CompressionAction)

  /// @group_operations

#define ELEFITS_DECLARE_VISIT(T, name) \
  /** @brief Create a compression algorithm according to some initializer. */ \
  virtual bool operator()(fitsfile* fptr, const ImageHdu::Initializer<T>& init) = 0;
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_DECLARE_VISIT)
#undef ELEFITS_DECLARE_VISIT

  /// @}
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

  /// @group_construction

  CompressionActionMixin() = default;
  ELEFITS_COPYABLE(CompressionActionMixin)
  ELEFITS_MOVABLE(CompressionActionMixin)
  ELEFITS_VIRTUAL_DTOR(CompressionActionMixin)

  /// @group_operations

#define ELEFITS_IMPLEMENT_VISIT(T, name) \
  /** @brief Compress if the strategy is compatible with the initializer. */ \
  bool operator()(fitsfile* fptr, const ImageHdu::Initializer<T>& init) override \
  { \
    return static_cast<TDerived&>(*this).apply(fptr, init); \
  }
  ELEFITS_FOREACH_RASTER_TYPE(ELEFITS_IMPLEMENT_VISIT)
#undef ELEFITS_IMPLEMENT_VISIT

  /// @}
};

/**
 * @ingroup compression
 * @brief A compression action made of a single algorithm.
 * @tparam TAlgo The compression algorithm
 */
template <typename TAlgo>
class Compress : public CompressionActionMixin<Compress<TAlgo>> {
public:

  /// @group_construction

  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit Compress(Ts&&... args) : m_algo(std::forward<Ts>(args)...)
  {}

  ELEFITS_VIRTUAL_DTOR(Compress)
  ELEFITS_COPYABLE(Compress)
  ELEFITS_MOVABLE(Compress)

  /// @group_operations

  /**
   * @brief Try compressing.
   * 
   * If the algorithm is not compatible with the initializer,
   * then no compression is performed and `false` is returned.
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init);

  /**
   * @brief Try creating a compression algorithm.
   * 
   * If the algorithm is not compatible with the initializer,
   * then `nullptr` is returned.
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init);

  /// @}

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

  /// @group_construction

  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit CompressInts(Ts&&... args) : m_compress(std::forward<Ts>(args)...)
  {}

  ELEFITS_COPYABLE(CompressInts)
  ELEFITS_MOVABLE(CompressInts)
  ELEFITS_VIRTUAL_DTOR(CompressInts)

  /// @group_operations

  /**
   * @copydoc Compress::apply()
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init)
  {
    if constexpr (not std::is_integral_v<T>) {
      return false;
    }
    return m_compress.apply(fptr, init);
  }

  /**
   * @copydoc Compress::compression()
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init)
  {
    if constexpr (not std::is_integral_v<T>) {
      return nullptr;
    }
    return m_compress.compression(init);
  }

  /// @}

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

  /// @group_construction

  /**
   * @brief Constructor.
   */
  template <typename... Ts>
  explicit CompressFloats(Ts&&... args) : m_compress(std::forward<Ts>(args)...)
  {}

  ELEFITS_COPYABLE(CompressFloats)
  ELEFITS_MOVABLE(CompressFloats)
  ELEFITS_VIRTUAL_DTOR(CompressFloats)

  /// @group_operations

  /**
   * @copydoc Compress::apply()
   */
  template <typename T>
  bool apply(fitsfile* fptr, const ImageHdu::Initializer<T>& init)
  {
    if constexpr (not std::is_floating_point_v<T>) {
      return false;
    }
    return m_compress.apply(fptr, init);
  }

  /**
   * @copydoc Compress::compression()
   */
  template <typename T>
  std::unique_ptr<TAlgo> compression(const ImageHdu::Initializer<T>& init)
  {
    if constexpr (not std::is_floating_point_v<T>) {
      return nullptr;
    }
    return m_compress.compression(init);
  }

  /// @}

private:

  /**
   * @brief The parent compression action.
   */
  Compress<TAlgo> m_compress;
};

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

  /// @group_construction

  /**
   * @brief Constructor.
   */
  explicit CompressAuto(CompressionType type = CompressionType::Lossless) : m_type(type) {}

  /// @group_operations

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

  /// @}

private:

  /**
   * @brief Adapt the quantization to the pixel type and strategy type.
   */
  template <typename T>
  Quantization quantization() const;

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
