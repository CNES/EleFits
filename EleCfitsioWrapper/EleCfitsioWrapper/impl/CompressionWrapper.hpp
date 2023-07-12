// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"

namespace Euclid {

namespace Fits {

template <typename TDerived>
void AlgoMixin<TDerived>::compress(void* fptr) const {
  Cfitsio::compress((fitsfile*)fptr, static_cast<const TDerived&>(*this));
}

} // namespace Fits

namespace Cfitsio {

bool isCompressing(fitsfile* fptr) {
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read compression type");
  return algo != int(NULL);
}

std::unique_ptr<Fits::Compression> readCompression(fitsfile* fptr) {

  // Read algo
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read compression type");
  if (algo == int(NULL)) {
    return std::make_unique<Fits::NoCompression>();
  }

  // Read tiling
  Fits::Position<-1> tiling(MAX_COMPRESS_DIM);
  std::fill(tiling.begin(), tiling.end(), 1); // FIXME useful?
  fits_get_tile_dim(fptr, MAX_COMPRESS_DIM, tiling.data(), &status);
  // FIXME remove useless 1's down to dimension 2
  // FIXME handle ROW and WHOLE as {-1, 1} and {-1}, respectively
  CfitsioError::mayThrow(status, fptr, "Cannot read compression tiling");

  // Read quantization
  float scale = 0;
  fits_get_quantize_level(fptr, &scale, &status);
  Fits::Compression::Quantization quantization(
      scale <= 0 ? Fits::Compression::Scaling(-scale) : Fits::Compression::rms / scale);
  if (quantization && HeaderIo::hasKeyword(fptr, "FZQMETHD")) {
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
  CfitsioError::mayThrow(status, fptr, "Cannot read compression quantization");

  if (algo == RICE_1) {
    return std::make_unique<Fits::Rice>(std::move(tiling), std::move(quantization));
  }
  if (algo == HCOMPRESS_1) {
    auto out = std::make_unique<Fits::HCompress>(Fits::Position<-1> {tiling[0], tiling[1]}, std::move(quantization));
    fits_get_hcomp_scale(fptr, &scale, &status);
    CfitsioError::mayThrow(status, fptr, "Cannot read H-compress scaling");
    out->scaling(scale <= 0 ? Fits::Compression::Scaling(-scale) : Fits::Compression::rms * scale);
    // FIXME smoothing?
    return out;
  }
  if (algo == PLIO_1) {
    return std::make_unique<Fits::Plio>(std::move(tiling), std::move(quantization));
  }
  if (algo == GZIP_1) {
    return std::make_unique<Fits::Gzip>(std::move(tiling), std::move(quantization));
  }
  if (algo == GZIP_2) {
    return std::make_unique<Fits::ShuffledGzip>(std::move(tiling), std::move(quantization));
  }
  throw Fits::FitsError("Unknown compression type");
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
  const auto value = quantization.level().value();
  switch (quantization.level().type()) {
    case Fits::Compression::Scaling::Type::Absolute:
      fits_set_quantize_level(fptr, -value, &status);
      break;
    case Fits::Compression::Scaling::Type::Factor:
      fits_set_quantize_level(fptr, 1. / value, &status);
      break;
    case Fits::Compression::Scaling::Type::Inverse:
      fits_set_quantize_level(fptr, value, &status);
      break;
  }
  CfitsioError::mayThrow(status, fptr, "Cannot set quantization level");

  // Set lossy int compression if quantization is enabled
  fits_set_lossy_int(fptr, quantization, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set lossy integer compression flag");

  // Set dithering method
  // fits_set_quantize_method() is the deprecated name of set_quantize_dither()
  switch (quantization.dithering()) {
    case Fits::Compression::Dithering::EveryPixel:
      fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_1, &status);
      break;
    case Fits::Compression::Dithering::NonZeroPixel:
      fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_2, &status);
      break;
    case Fits::Compression::Dithering::None:
      fits_set_quantize_dither(fptr, NO_DITHER, &status);
      break;
  }
  CfitsioError::mayThrow(status, fptr, "Cannot set dithering method");
}

void compress(fitsfile* fptr, const Fits::NoCompression&) {
  int status = 0;
  fits_set_compression_type(fptr, int(NULL), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to NoCompression");
}

void compress(fitsfile* fptr, const Fits::Gzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Gzip");
  setTiling(fptr, algo.tiling());
  setQuantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::ShuffledGzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to ShuffledGzip");
  setTiling(fptr, algo.tiling());
  setQuantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::Rice& algo) {
  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Rice");
  setTiling(fptr, algo.tiling());
  setQuantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::HCompress& algo) {

  int status = 0;

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to HCompress");
  setTiling(fptr, algo.tiling());
  setQuantize(fptr, algo.quantization());

  const auto value = algo.scaling().value();
  switch (algo.scaling().type()) {
    case Fits::Compression::Scaling::Type::Absolute:
      fits_set_hcomp_scale(fptr, -value, &status);
      break;
    case Fits::Compression::Scaling::Type::Factor:
      fits_set_hcomp_scale(fptr, value, &status);
      break;
    case Fits::Compression::Scaling::Type::Inverse:
      fits_set_hcomp_scale(fptr, 1. / value, &status);
      break;
  }
  CfitsioError::mayThrow(status, fptr, "Cannot set H-compress scale");

  fits_set_hcomp_smooth(fptr, algo.isSmooth(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set H-compress smoothing recommendation");
}

void compress(fitsfile* fptr, const Fits::Plio& algo) {
  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot set compression type to Plio");
  setTiling(fptr, algo.tiling());
  setQuantize(fptr, algo.quantization());
}

} // namespace Cfitsio
} // namespace Euclid

#endif
