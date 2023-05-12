// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"

namespace Euclid {
namespace Fits {
namespace Compression {

template <typename TDerived, long N>
void AlgoMixin<TDerived, N>::compress(fitsfile* fptr) const {

  int status = 0;

  Position<N> ndims = m_shape;
  fits_set_tile_dim(fptr, N, ndims.data(), &status); // setting compression tile size
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  Euclid::Cfitsio::Compression::compress(fptr, static_cast<const TDerived&>(*this));
}

template <typename TDerived, long N>
void FloatAlgo<TDerived, N>::compress(fitsfile* fptr) const {

  int status = 0;

  // setting quantize level:
  if (!quantize().isAbsolute()) {
    fits_set_quantize_level(fptr, quantize().level(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative quantize level");
  } else { // absolute quantize level applied in this case
    fits_set_quantize_level(fptr, -quantize().level(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute quantize level");
  }

  // setting dither method:
  if (dither() == Dithering::NoDithering) {

    fits_set_quantize_dither(fptr, NO_DITHER, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NoDithering");

  } else if (dither() == Dithering::NonZeroPixelDithering) {

    fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_2, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NonZeroPixelDithering");

  } else { // Dithering::EveryPixelDithering in this case

    fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_1, &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to EveryPixelDithering");
  }

  // setting lossy int compression:
  // FIXME: how to verify that it is applied correctly to img?
  fits_set_lossy_int(fptr, lossyInt(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set lossy int compression");

  Euclid::Cfitsio::Compression::compress(fptr, static_cast<const TDerived&>(*this));
}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

namespace Euclid {
namespace Cfitsio {
namespace Compression {

void compress(fitsfile* fptr, const Euclid::Fits::Compression::None& algo) {

  (void)algo; // no params for None

  int status = 0;
  fits_set_compression_type(fptr, NULL, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to None");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Rice<N>& algo) {

  (void)algo; // can be used later for algo params specific to Rice

  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::HCompress<N>& algo) {

  int status = 0;

  if (!algo.scale().isAbsolute()) {
    fits_set_hcomp_scale(fptr, algo.scale().factor(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative scale for HCompress");
  } else { // absolute scaling applied in this case
    fits_set_hcomp_scale(fptr, -algo.scale().factor(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute scale for HCompress");
  }

  fits_set_hcomp_smooth(fptr, algo.isSmooth(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set smoothing for HCompress");

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Plio<N>& algo) {

  (void)algo; // can be used later for algo params specific to Plio

  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Gzip<N>& algo) {

  (void)algo; // can be used later for algo params specific to Gzip

  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::ShuffledGzip<N>& algo) {

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
