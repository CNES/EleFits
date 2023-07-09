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
template <typename TDerived>
void AlgoMixin<TDerived>::compress(void* fptr) const {
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
  // FIXME handle ROW and WHOLE
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
      out.reset(new Fits::Compression::Rice(tiling));
      break;
    case HCOMPRESS_1:
      out.reset(new Fits::Compression::HCompress({tiling[0], tiling[1]}));
      fits_get_hcomp_scale(fptr, &factor, &status);
      dynamic_cast<Fits::Compression::HCompress&>(*out).scale(std::move(scaling)).quantization(std::move(quantization));
      break;
    case PLIO_1:
      out.reset(new Fits::Compression::Plio(tiling));
      dynamic_cast<Fits::Compression::Plio&>(*out).quantization(std::move(quantization));
      break;
    case GZIP_1:
      out.reset(new Fits::Compression::Gzip(tiling));
      dynamic_cast<Fits::Compression::Gzip&>(*out).quantization(std::move(quantization));
      break;
    case GZIP_2:
      out.reset(new Fits::Compression::ShuffledGzip(tiling));
      dynamic_cast<Fits::Compression::ShuffledGzip&>(*out).quantization(std::move(quantization));
      break;
    default:
      throw Fits::FitsError("Unknown compression type");
  }
  CfitsioError::mayThrow(status, fptr, "Cannot read compression parameters");

  return out;
}

inline void setTiling(fitsfile* fptr, const Fits::Position<-1>& shape) {
  int status = 0;
  Euclid::Fits::Position<-1> nonconstShape = shape;
  fits_set_tile_dim(fptr, nonconstShape.size(), nonconstShape.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression tiling");
}

inline void setQuantize(fitsfile* fptr, const Fits::Compression::Quantization& quantization) {

  int status = 0;

  // Set quantization level
  if (quantization.level().type() == Fits::Compression::Factor::Type::Absolute) {
    fits_set_quantize_level(fptr, -quantization.level().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set absolute quantization level");
  } else { // relative quantization level applied in this case
    fits_set_quantize_level(fptr, quantization.level().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set relative quantization level");
  }

  // Set dithering method
  // fits_set_quantize_method() is exact same as set_quantize_dither() (.._method() is the old name)
  switch (quantization.dithering()) {
    case Fits::Compression::Dithering::EveryPixel:
      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_1, &status);
      break;
    case Fits::Compression::Dithering::NonZeroPixel:
      fits_set_quantize_method(fptr, SUBTRACTIVE_DITHER_2, &status);
      break;
    case Fits::Compression::Dithering::None:
      fits_set_quantize_method(fptr, NO_DITHER, &status);
      break;
  }
  CfitsioError::mayThrow(status, fptr, "Cannot set dithering method");

  // Set lossy int compression
  fits_set_lossy_int(fptr, quantization.hasLossyInt(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set lossy integer compression flag");
}

void compress(fitsfile* fptr, const Fits::Compression::None&) {
  int status = 0;
  fits_set_compression_type(fptr, int(NULL), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to None");
}

void compress(fitsfile* fptr, const Fits::Compression::Rice& algo) {
  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");
  setTiling(fptr, algo.shape());
  setQuantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::Compression::HCompress& algo) {

  int status = 0;

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");

  setTiling(fptr, algo.shape());
  setQuantize(fptr, algo.quantization());

  if (algo.scale().type() == Fits::Compression::Factor::Type::Absolute) {
    fits_set_hcomp_scale(fptr, -algo.scale().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set absolute scale for HCompress");
  } else { // None or relative scaling applied in this case
    fits_set_hcomp_scale(fptr, algo.scale().value(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot set relative scale for HCompress");
  }

  fits_set_hcomp_smooth(fptr, algo.isSmooth(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set smoothing for HCompress");
}

void compress(fitsfile* fptr, const Fits::Compression::Plio& algo) {
  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");
  setTiling(fptr, algo.shape());
}

void compress(fitsfile* fptr, const Fits::Compression::Gzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");
  setTiling(fptr, algo.shape());
  setQuantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::Compression::ShuffledGzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to ShuffledGzip");
  setTiling(fptr, algo.shape());
  setQuantize(fptr, algo.quantization());
}

} // namespace Compression
} // namespace Cfitsio
} // namespace Euclid

#endif
