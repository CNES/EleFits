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
 * @brief Know if the cfitsio image compression is turned on.
 */
inline bool isCompressing(fitsfile* fptr);

/**
 * @brief Set the compression algorithm to None.
 */
inline void compress(fitsfile* fptr, const Euclid::Fits::Compression::None& algo);

/**
 * @brief Set the compression algorithm to Rice.
 */
template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Rice<N>& algo);

/**
 * @brief Set the compression algorithm to HCompress.
 */
inline void compress(fitsfile* fptr, const Euclid::Fits::Compression::HCompress& algo);

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

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define COMPRESSIONWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/CompressionWrapper.hpp"
#undef COMPRESSIONWRAPPER_IMPL
/// @endcond

#endif