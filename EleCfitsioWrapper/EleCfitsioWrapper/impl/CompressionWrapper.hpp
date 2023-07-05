// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"

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
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read compression type");
  return algo != int(NULL);
}

std::unique_ptr<Fits::Compression::Algo> readCompression(fitsfile* fptr) {

  // Read algo
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read compression type");
  std::unique_ptr<Fits::Compression::Algo> out(new Fits::Compression::None());
  if (algo == int(NULL)) {
    return out;
  }

  // Read tiling
  Fits::Position<-1> tiling(MAX_COMPRESS_DIM);
  std::fill(tiling.begin(), tiling.end(), 1); // FIXME useful?
  fits_get_tile_dim(fptr, MAX_COMPRESS_DIM, tiling.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read compression tiling");

  // Read quantization
  float factor = 0;
  auto factorize = [](float f) {
    if (f == 0) {
      return Fits::Compression::Factor::none();
    } else if (f < 0) {
      return Fits::Compression::Factor::absolute(-f);
    } else {
      return Fits::Compression::Factor::relative(f);
    }
  };
  fits_get_quantize_level(fptr, &factor, &status);
  Fits::Compression::Quantization quantization = factorize(factor);
  if (HeaderIo::hasKeyword(fptr, "FZQMETHD")) {
    const std::string method = HeaderIo::parseRecord<std::string>(fptr, "FZQMETHD");
    if (method == "NO_DITHER") {
      quantization.dithering(Fits::Compression::Dithering::None);
    } else if (method == "SUBTRACTIVE_DITHER_1") {
      quantization.dithering(Fits::Compression::Dithering::EveryPixel);
    } else if (method == "SUBTRACTIVE_DITHER_2") {
      quantization.dithering(Fits::Compression::Dithering::NonZeroPixel);
    } else {
      Fits::FitsError(std::string("Unknown compression dithering method: ") + method);
    }
  }
  if (HeaderIo::hasKeyword(fptr, "FZINT2F") && HeaderIo::parseRecord<bool>(fptr, "FZINT2F")) {
    quantization.enableLossyInt();
  } else {
    quantization.disableLossyInt();
  }
  CfitsioError::mayThrow(status, fptr, "Cannot read compression quantization");

  // Read scaling
  fits_get_hcomp_scale(fptr, &factor, &status);
  auto scaling = factorize(factor);
  // FIXME smoothing?
  CfitsioError::mayThrow(status, fptr, "Cannot read compression scaling");

  switch (algo) {
    case RICE_1:
      out.reset(new Fits::Compression::Rice<-1>(tiling));
      break;
    case HCOMPRESS_1:
      out.reset(new Fits::Compression::HCompress({tiling[0], tiling[1]}));
      fits_get_hcomp_scale(fptr, &factor, &status);
      dynamic_cast<Fits::Compression::HCompress&>(*out).scale(std::move(scaling)).quantize(std::move(quantization));
      break;
    case PLIO_1:
      out.reset(new Fits::Compression::Plio<-1>(tiling));
      dynamic_cast<Fits::Compression::Plio<-1>&>(*out).quantize(std::move(quantization));
      break;
    case GZIP_1:
      out.reset(new Fits::Compression::Gzip<-1>(tiling));
      dynamic_cast<Fits::Compression::Gzip<-1>&>(*out).quantize(std::move(quantization));
      break;
    case GZIP_2:
      out.reset(new Fits::Compression::ShuffledGzip<-1>(tiling));
      dynamic_cast<Fits::Compression::ShuffledGzip<-1>&>(*out).quantize(std::move(quantization));
      break;
    default:
      throw Fits::FitsError("Unknown compression type");
  }
  CfitsioError::mayThrow(status, fptr, "Cannot read compression parameters");

  return out;
}

// function to factorize quantization for all floating-point algorithms
inline void setQuantize(fitsfile* fptr, const Fits::Compression::Quantization& quant) {

  int status = 0;

  // setting quantize level:
  if (quant.level().type() == Fits::Compression::Factor::Type::Absolute) {
    fits_set_quantize_level(fptr, -quant.level().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set absolute quantize level");
  } else { // relative quantize level applied in this case
    fits_set_quantize_level(fptr, quant.level().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set relative quantize level");
  }

  // setting dithering method:
  // fits_set_quantize_method() is exact same as set_quantize_dither() (.._method() is the old name)
  switch (quant.dithering()) {
    case Fits::Compression::Dithering::EveryPixel:

      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_1, &status);
      CfitsioError::mayThrow(status, fptr, "Cannot set dithering to EveryPixel");
      break;

    case Fits::Compression::Dithering::NonZeroPixel:

      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_2, &status);
      CfitsioError::mayThrow(status, fptr, "Cannot set dithering to NonZeroPixel");
      break;

    case Fits::Compression::Dithering::None:

      fits_set_quantize_method(fptr, NO_DITHER, &status);
      CfitsioError::mayThrow(status, fptr, "Cannot set dithering to None");
      break;
  }

  // setting lossy int compression:
  // FIXME: how to verify that it is applied correctly to img?
  fits_set_lossy_int(fptr, quant.hasLossyInt(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set lossy int compression");
}

void compress(fitsfile* fptr, const Fits::Compression::None&) {

  int status = 0;
  fits_set_compression_type(fptr, int(NULL), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to None");
}

template <long N>
void compress(fitsfile* fptr, const Fits::Compression::Rice<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

void compress(fitsfile* fptr, const Fits::Compression::HCompress& algo) {

  int status = 0;

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");

  Euclid::Fits::Position<2> ndims = algo.shape();
  fits_set_tile_dim(fptr, 2, ndims.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  if (algo.scale().type() == Fits::Compression::Factor::Type::Absolute) {
    fits_set_hcomp_scale(fptr, -algo.scale().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set absolute scale for HCompress");
  } else { // relative scaling applied in this case
    fits_set_hcomp_scale(fptr, algo.scale().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set relative scale for HCompress");
  }

  fits_set_hcomp_smooth(fptr, algo.isSmooth(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set smoothing for HCompress");

  setQuantize(fptr, algo.quantize());
}

template <long N>
void compress(fitsfile* fptr, const Fits::Compression::Plio<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");
}

template <long N>
void compress(fitsfile* fptr, const Fits::Compression::Gzip<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

template <long N>
void compress(fitsfile* fptr, const Fits::Compression::ShuffledGzip<N>& algo) {

  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to ShuffledGzip");

  Euclid::Fits::Position<N> ndims = algo.shape();
  fits_set_tile_dim(fptr, N, ndims.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tile dimensions");

  setQuantize(fptr, algo.quantize());
}

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

#endif
