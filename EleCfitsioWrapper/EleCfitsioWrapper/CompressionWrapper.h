// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSIONWRAPPER_H
#define COMPRESSIONWRAPPER_H

#include "EleFitsData/Compression.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Cfitsio {
namespace Compression {

/**
 * @brief Set the compression algorithm to None.
 */
void compress(fitsfile* fptr, const Euclid::Fits::Compression::None& algo);

/**
 * @brief Set the compression algorithm to Rice.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Rice<N>& algo);

/**
 * @brief Set the compression algorithm to HCompress.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::HCompress<N>& algo);

/**
 * @brief Set the compression algorithm to Plio.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Plio<N>& algo);

/**
 * @brief Set the compression algorithm to Gzip.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Gzip<N>& algo);

/**
 * @brief Set the compression algorithm to ShuffledGzip.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::ShuffledGzip<N>& algo);

/**
 * @brief Compression traits to know which C++ type supports which algorithm.
 */
template <typename T>
struct CompressionTraits {

  /**
   * @brief Get if type has support for compression.
   */
  inline static bool has();
};

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define COMPRESSIONWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/CompressionWrapper.hpp"
#undef COMPRESSIONWRAPPER_IMPL
/// @endcond

#endif