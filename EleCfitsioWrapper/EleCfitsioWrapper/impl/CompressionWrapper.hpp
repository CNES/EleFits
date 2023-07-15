// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsUtils/StringUtils.h"

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

template <typename T>
T parseValueOr(fitsfile* fptr, const std::string& key, T fallback) {
  auto name = [](int i) {
    return std::string("ZNAME") + std::to_string(i);
  };
  auto val = [](int i) {
    return std::string("ZVAL") + std::to_string(i);
  };
  for (int i = 1; HeaderIo::hasKeyword(fptr, name(i)); ++i) {
    const std::string value = HeaderIo::parseRecord<std::string>(fptr, name(i)).value;
    if (value == key) {
      return HeaderIo::parseRecord<T>(fptr, val(i));
    }
  }
  return fallback;
}

std::unique_ptr<Fits::Compression> readCompression(fitsfile* fptr) {

  /* Is compressed? */

  if (not HeaderIo::hasKeyword(fptr, "ZCMPTYPE")) {
    return std::make_unique<Fits::NoCompression>();
  }
  const std::string name = HeaderIo::parseRecord<std::string>(fptr, "ZCMPTYPE");
  if (name == "NONE" || name == "NOCOMPRESS") {
    return std::make_unique<Fits::NoCompression>();
  }
  // FIXME use ZIMAGE?

  /* Tiling */

  const auto dimension = std::min<long>(HeaderIo::parseRecord<long>(fptr, "ZNAXIS"), MAX_COMPRESS_DIM);
  Fits::Position<-1> shape(dimension);
  for (long i = 0; i < dimension; ++i) {
    shape[i] = 1;
    try {
      shape[i] = HeaderIo::parseRecord<long>(fptr, std::string("ZTILE") + std::to_string(i + 1));
    } catch (CfitsioError&) {
    }
  }
  // FIXME check that ZTILE7 does not exist
  // FIXME remove trailing 1's
  // FIXME detect rowwiseTiling and maxTiling

  /* Quantization */

  bool quantized = false;
  const long columnCount = HeaderIo::parseRecord<long>(fptr, "TFIELDS");
  for (long i = 1; i <= columnCount; ++i) {
    const std::string columnName = HeaderIo::parseRecord<std::string>(fptr, std::string("TTYPE") + std::to_string(i));
    if (columnName == "ZSCALE") {
      quantized = true;
    }
  }
  const auto level = parseValueOr<double>(fptr, "NOISEBIT", quantized ? 4 : 0);
  // FIXME not set by CFITSIO (but set by astropy)
  Fits::Compression::Quantization quantization(
      level <= 0 ? Fits::Compression::Scaling(-level) : Fits::Compression::rms / level);

  // FIXME seed

  /* Dithering */

  if (HeaderIo::hasKeyword(fptr, "ZQUANTIZ")) {
    const std::string method = HeaderIo::parseRecord<std::string>(fptr, "ZQUANTIZ");
    if (method == "NONE") {
      // NONE is not standard but happens to indicate null quantization (level = 0)
      quantization = Fits::Compression::Quantization(0);
    } else if (method == "NO_DITHER") {
      quantization.dithering(Fits::Compression::Dithering::None);
    } else if (method == "SUBTRACTIVE_DITHER_1") {
      quantization.dithering(Fits::Compression::Dithering::EveryPixel);
    } else if (method == "SUBTRACTIVE_DITHER_2") {
      quantization.dithering(Fits::Compression::Dithering::NonZeroPixel);
    } else {
      Fits::FitsError(std::string("Unknown compression dithering method: ") + method);
    }
  } else {
    quantization.dithering(Fits::Compression::Dithering::None);
  }

  if (name == "GZIP_1") {
    return std::make_unique<Fits::Gzip>(std::move(shape), std::move(quantization));
  }
  if (name == "GZIP_2") {
    return std::make_unique<Fits::ShuffledGzip>(std::move(shape), std::move(quantization));
  }
  if (name == "RICE_1") {
    return std::make_unique<Fits::Rice>(std::move(shape), std::move(quantization));
  }
  if (name == "HCOMPRESS_1") {
    const auto scale = parseValueOr<double>(fptr, "SCALE", 0);
    auto out = std::make_unique<Fits::HCompress>(Fits::Position<-1> {shape[0], shape[1]}, std::move(quantization));
    out->scaling(scale <= 0 ? Fits::Compression::Scaling(-scale) : Fits::Compression::rms * scale);
    return out;
  }
  if (name == "PLIO_1") {
    return std::make_unique<Fits::Plio>(std::move(shape), std::move(quantization));
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
