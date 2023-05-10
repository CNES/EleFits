// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Compression {

void compress(fitsfile* fptr, Euclid::Fits::Compression::None algo) {

  (void)algo; // no params for None

  int status = 0;
  fits_set_compression_type(fptr, NULL, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to None");
}

template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Rice<N> algo) {

  (void)algo; // can be used later for algo params specific to Rice

  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");
}

template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::HCompress<N> algo) {

  int status = 0;

  if (!algo.scale().isAbsolute()) {
    fits_set_hcomp_scale(fptr, algo.scale().factor(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative scale for HCompress");
  } else { // absolute scaling applied in this case
    fits_set_hcomp_scale(fptr, -algo.scale().factor(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute scale for HCompress");
  }

  fits_set_hcomp_smooth(fptr, algo.smooth(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set smoothing for HCompress");

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");
}

template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Plio<N> algo) {

  (void)algo; // can be used later for algo params specific to Plio

  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");
}

template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::Gzip<N> algo) {

  (void)algo; // can be used later for algo params specific to Gzip

  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");
}

template <long N>
void compress(fitsfile* fptr, Euclid::Fits::Compression::ShuffledGzip<N> algo) {

  (void)algo; // can be used later for algo params specific to Gzip2

  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to ShuffledGzip");
}

#ifndef DEF_HAS_COMPRESSION
#define DEF_HAS_COMPRESSION(type, compressionSupported) \
  template <> \
  inline bool CompressionTraits<type>::has() { \
    return compressionSupported; \
  } \
  template <> \
  inline bool CompressionTraits<const type>::has() { \
    return compressionSupported; \
  }
DEF_HAS_COMPRESSION(char, true)
DEF_HAS_COMPRESSION(std::int16_t, true)
DEF_HAS_COMPRESSION(std::int32_t, true)
DEF_HAS_COMPRESSION(std::int64_t, false)
DEF_HAS_COMPRESSION(float, true)
DEF_HAS_COMPRESSION(double, true)
DEF_HAS_COMPRESSION(unsigned char, true)
DEF_HAS_COMPRESSION(std::uint16_t, true)
DEF_HAS_COMPRESSION(std::uint32_t, true)
DEF_HAS_COMPRESSION(std::uint64_t, false)
#undef DEF_IMAGE_BITPIX
#endif

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

#endif
