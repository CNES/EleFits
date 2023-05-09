// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef COMPRESSIONWRAPPER_H
#define COMPRESSIONWRAPPER_H

#include <string>

// class None;
// template <long N>
// class Rice;
// template <long N>
// class HCompress;
// template <long N>
// class Plio;
// template <long N>
// class Gzip;
// template <long N>
// class ShuffledGzip;

#include <fitsio.h>
#include "EleFitsData/CompressionData.h"

namespace Euclid {
namespace Cfitsio {
namespace Compression {

/**
 * @brief Set the compression algorithm to None.
 */
void compress(fitsfile* fptr, None noneAlgo);

/**
 * @brief Set the compression algorithm to Rice.
 */
template <long N>
void compress(fitsfile* fptr, Rice<N> riceAlgo);

/**
 * @brief Set the compression algorithm to HCompress.
 */
template <long N>
void compress(fitsfile* fptr, HCompress<N> hcompressAlgo);

/**
 * @brief Set the compression algorithm to Plio.
 */
template <long N>
void compress(fitsfile* fptr, Plio<N> plioAlgo);

/**
 * @brief Set the compression algorithm to Gzip.
 */
template <long N>
void compress(fitsfile* fptr, Gzip<N> gzipAlgo);

/**
 * @brief Set the compression algorithm to ShuffledGzip.
 */
template <long N>
void compress(fitsfile* fptr, ShuffledGzip<N> shuffledGzipAlgo);

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

}  // namespace Compression
}  // namespace Cfitsio
}  // namespace Euclid

/// @cond INTERNAL
#define COMPRESSIONWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/CompressionWrapper.hpp"
#undef COMPRESSIONWRAPPER_IMPL
/// @endcond

#endif