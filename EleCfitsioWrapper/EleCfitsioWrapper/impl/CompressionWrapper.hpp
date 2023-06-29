// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"

namespace Euclid {
namespace Fits {
namespace Compression {

// used to dispatch the compress() call for each AlgoMixin subclass TDerived
template <long N, typename TDerived>
void AlgoMixin<N, TDerived>::compress(void* fptr) const {
  Euclid::Cfitsio::Compression::compress((fitsfile*)fptr, static_cast<const TDerived&>(*this));
}

} // namespace Compression
} // namespace Fits
} // namespace Euclid

namespace Euclid {
namespace Cfitsio {
namespace Compression {

bool isCompressing(fitsfile* fptr) {

  int currentAlgo;
  int status = 0;

  fits_get_compression_type(fptr, &currentAlgo, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot get compression type");

  return currentAlgo != int(NULL);
}

// function to factorize quantization for all floating-point algorithms
inline void setQuantize(fitsfile* fptr, const Euclid::Fits::Compression::Quantization& quant) {

  int status = 0;

  // setting quantize level:
  if (quant.level().type() == Euclid::Fits::Compression::Factor::Type::Absolute) {
    fits_set_quantize_level(fptr, -quant.level().value(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute quantize level");
  } else { // relative quantize level applied in this case
    fits_set_quantize_level(fptr, quant.level().value(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative quantize level");
  }

  // setting dithering method:
  // fits_set_quantize_method() is exact same as set_quantize_dither() (.._method() is the old name)
  switch (quant.dithering()) {
    case Euclid::Fits::Compression::Dithering::EveryPixel:

      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_1, &status);
      Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to EveryPixel");
      break;

    case Euclid::Fits::Compression::Dithering::NonZeroPixel:

      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_2, &status);
      Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NonZeroPixel");
      break;

    case Euclid::Fits::Compression::Dithering::None:

      fits_set_quantize_method(fptr, NO_DITHER, &status);
      Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set dithering to None");
      break;
  }

  // setting lossy int compression:
  // FIXME: how to verify that it is applied correctly to img?
  fits_set_lossy_int(fptr, quant.hasLossyInt(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set lossy int compression");
}

void compress(fitsfile* fptr, const Euclid::Fits::Compression::None&) {

  int status = 0;
  fits_set_compression_type(fptr, int(NULL), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to None");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Rice<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

void compress(fitsfile* fptr, const Euclid::Fits::Compression::HCompress& algo) {

  int status = 0;

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");

  Euclid::Fits::Position<2> ndims = algo.shape();
  fits_set_tile_dim(fptr, 2, ndims.data(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  if (algo.scale().type() == Euclid::Fits::Compression::Factor::Type::Absolute) {
    fits_set_hcomp_scale(fptr, -algo.scale().value(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set absolute scale for HCompress");
  } else { // relative scaling applied in this case
    fits_set_hcomp_scale(fptr, algo.scale().value(), &status);
    Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set relative scale for HCompress");
  }

  fits_set_hcomp_smooth(fptr, algo.isSmooth(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set smoothing for HCompress");

  setQuantize(fptr, algo.quantize());
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Plio<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::Gzip<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

template <long N>
void compress(fitsfile* fptr, const Euclid::Fits::Compression::ShuffledGzip<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression type to ShuffledGzip");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  Euclid::Cfitsio::CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

#endif
