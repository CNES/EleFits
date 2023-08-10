// Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_COMPRESSIONWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/CompressionWrapper.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFitsUtils/StringUtils.h"

namespace Euclid {
namespace Cfitsio {
namespace ImageCompression {

bool is_compressing(fitsfile* fptr) {
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read compression type");
  return algo != int(NULL);
}

std::unique_ptr<Fits::Compression> get_compression(fitsfile* fptr) {

  // Read algo
  int status = 0;
  int algo = int(NULL);
  fits_get_compression_type(fptr, &algo, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read compression type");
  if (algo == int(NULL)) {
    return std::make_unique<Fits::NoCompression>();
  }

  // Read tiling
  Fits::Position<-1> tiling(MAX_COMPRESS_DIM);
  std::fill(tiling.begin(), tiling.end(), 1); // FIXME useful?
  fits_get_tile_dim(fptr, MAX_COMPRESS_DIM, tiling.data(), &status);
  // FIXME remove useless 1's down to dimension 2
  // FIXME handle ROW and WHOLE as {-1, 1} and {-1}, respectively
  CfitsioError::may_throw(status, fptr, "Cannot read compression tiling");

  // Read quantization
  float scale = 0;
  fits_get_quantize_level(fptr, &scale, &status);
  Fits::Quantization quantization(scale <= 0 ? Fits::Scaling(-scale) : Fits::Tile::rms / scale);
  CfitsioError::may_throw(status, fptr, "Cannot read compression quantization");

  if (algo == RICE_1) {
    return std::make_unique<Fits::Rice>(std::move(tiling), std::move(quantization));
  }
  if (algo == HCOMPRESS_1) {
    auto out = std::make_unique<Fits::HCompress>(Fits::Position<-1> {tiling[0], tiling[1]}, std::move(quantization));
    fits_get_hcomp_scale(fptr, &scale, &status);
    CfitsioError::may_throw(status, fptr, "Cannot read H-compress scaling");
    out->scaling(scale <= 0 ? Fits::Scaling(-scale) : Fits::Tile::rms * scale);
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

template <typename T>
T parse_value_or(fitsfile* fptr, const std::string& key, T fallback) {
  auto name = [](int i) {
    return std::string("ZNAME") + std::to_string(i);
  };
  auto val = [](int i) {
    return std::string("ZVAL") + std::to_string(i);
  };
  for (int i = 1; HeaderIo::has_keyword(fptr, name(i)); ++i) {
    const std::string value = HeaderIo::parse_record<std::string>(fptr, name(i)).value;
    if (value == key) {
      return HeaderIo::parse_record<T>(fptr, val(i));
    }
  }
  return fallback;
}

std::unique_ptr<Fits::Compression> read_parameters(fitsfile* fptr) {

  /* Is compressed? */

  if (not HeaderIo::has_keyword(fptr, "ZCMPTYPE")) {
    return std::make_unique<Fits::NoCompression>();
  }
  const std::string name = HeaderIo::parse_record<std::string>(fptr, "ZCMPTYPE");
  if (name == "NONE" || name == "NOCOMPRESS") {
    return std::make_unique<Fits::NoCompression>();
  }
  // FIXME use ZIMAGE?

  /* Tiling */

  const auto dimension = std::min<long>(HeaderIo::parse_record<long>(fptr, "ZNAXIS"), MAX_COMPRESS_DIM);
  Fits::Position<-1> shape(dimension);
  for (long i = 0; i < dimension; ++i) {
    shape[i] = 1;
    try {
      shape[i] = HeaderIo::parse_record<long>(fptr, std::string("ZTILE") + std::to_string(i + 1));
    } catch (CfitsioError&) {
    }
  }
  // FIXME check that ZTILE7 does not exist
  // FIXME remove trailing 1's
  // FIXME detect Tile::rowwise() and Tile::whole()

  /* Quantization */

  bool quantized = false;
  const long column_count = HeaderIo::parse_record<long>(fptr, "TFIELDS");
  for (long i = 1; i <= column_count; ++i) {
    const std::string column_name = HeaderIo::parse_record<std::string>(fptr, std::string("TTYPE") + std::to_string(i));
    if (column_name == "ZSCALE") {
      quantized = true;
    }
  }
  const auto level = parse_value_or<double>(fptr, "NOISEBIT", quantized ? 4 : 0);
  // FIXME not set by CFITSIO (but set by astropy)
  Fits::Quantization quantization(level <= 0 ? Fits::Scaling(-level) : Fits::Tile::rms / level);

  // FIXME seed

  /* Dithering */

  if (HeaderIo::has_keyword(fptr, "ZQUANTIZ")) {
    const std::string method = HeaderIo::parse_record<std::string>(fptr, "ZQUANTIZ");
    if (method == "NONE") {
      // NONE is not standard but happens to indicate null quantization (level = 0)
      quantization = Fits::Quantization(0);
    } else if (method == "NO_DITHER") {
      quantization.dithering(Fits::Quantization::Dithering::None);
    } else if (method == "SUBTRACTIVE_DITHER_1") {
      quantization.dithering(Fits::Quantization::Dithering::EveryPixel);
    } else if (method == "SUBTRACTIVE_DITHER_2") {
      quantization.dithering(Fits::Quantization::Dithering::NonZeroPixel);
    } else {
      Fits::FitsError(std::string("Unknown compression dithering method: ") + method);
    }
  } else {
    quantization.dithering(Fits::Quantization::Dithering::None);
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
    const auto scale = parse_value_or<double>(fptr, "SCALE", 0);
    auto out = std::make_unique<Fits::HCompress>(Fits::Position<-1> {shape[0], shape[1]}, std::move(quantization));
    out->scaling(scale <= 0 ? Fits::Scaling(-scale) : Fits::Tile::rms * scale);
    return out;
  }
  if (name == "PLIO_1") {
    return std::make_unique<Fits::Plio>(std::move(shape), std::move(quantization));
  }

  throw Fits::FitsError("Unknown compression type");
}

void enable_huge_compression(fitsfile* fptr, bool huge) {
  int status = 0;
  fits_set_huge_hdu(fptr, huge, &status);
  Cfitsio::CfitsioError::may_throw(status, fptr, "Cannot set huge HDU");
}

inline void set_tiling(fitsfile* fptr, const Fits::Position<-1>& shape) {
  int status = 0;
  Euclid::Fits::Position<-1> nonconst_shape = shape;
  fits_set_tile_dim(fptr, nonconst_shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression tiling");
}

inline void set_quantize(fitsfile* fptr, const Fits::Quantization& quantization) {

  int status = 0;

  // Set quantization level
  const auto value = quantization.level().value();
  switch (quantization.level().type()) {
    case Fits::Scaling::Type::Absolute:
      fits_set_quantize_level(fptr, -value, &status);
      break;
    case Fits::Scaling::Type::Factor:
      fits_set_quantize_level(fptr, 1. / value, &status);
      break;
    case Fits::Scaling::Type::Inverse:
      fits_set_quantize_level(fptr, value, &status);
      break;
  }
  CfitsioError::may_throw(status, fptr, "Cannot set quantization level");

  // Set lossy int compression if quantization is enabled
  fits_set_lossy_int(fptr, bool(quantization), &status);
  CfitsioError::may_throw(status, fptr, "Cannot set lossy integer compression flag");

  // Set dithering method
  // fits_set_quantize_method() is the deprecated name of set_quantize_dither()
  switch (quantization.dithering()) {
    case Fits::Quantization::Dithering::EveryPixel:
      fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_1, &status);
      break;
    case Fits::Quantization::Dithering::NonZeroPixel:
      fits_set_quantize_dither(fptr, SUBTRACTIVE_DITHER_2, &status);
      break;
    case Fits::Quantization::Dithering::None:
      fits_set_quantize_dither(fptr, NO_DITHER, &status);
      break;
  }
  CfitsioError::may_throw(status, fptr, "Cannot set dithering method");
}

void compress(fitsfile* fptr, const Fits::NoCompression&) {
  int status = 0;
  fits_set_compression_type(fptr, int(NULL), &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to NoCompression");
}

void compress(fitsfile* fptr, const Fits::Gzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_1, &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to Gzip");
  set_tiling(fptr, algo.tiling());
  set_quantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::ShuffledGzip& algo) {
  int status = 0;
  fits_set_compression_type(fptr, GZIP_2, &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to ShuffledGzip");
  set_tiling(fptr, algo.tiling());
  set_quantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::Rice& algo) {
  int status = 0;
  fits_set_compression_type(fptr, RICE_1, &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to Rice");
  set_tiling(fptr, algo.tiling());
  set_quantize(fptr, algo.quantization());
}

void compress(fitsfile* fptr, const Fits::HCompress& algo) {

  int status = 0;

  fits_set_compression_type(fptr, HCOMPRESS_1, &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to HCompress");
  set_tiling(fptr, algo.tiling());
  set_quantize(fptr, algo.quantization());

  const auto value = algo.scaling().value();
  switch (algo.scaling().type()) {
    case Fits::Scaling::Type::Absolute:
      fits_set_hcomp_scale(fptr, -value, &status);
      break;
    case Fits::Scaling::Type::Factor:
      fits_set_hcomp_scale(fptr, value, &status);
      break;
    case Fits::Scaling::Type::Inverse:
      fits_set_hcomp_scale(fptr, 1. / value, &status);
      break;
  }
  CfitsioError::may_throw(status, fptr, "Cannot set H-compress scale");

  fits_set_hcomp_smooth(fptr, algo.is_smooth(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot set H-compress smoothing recommendation");
}

void compress(fitsfile* fptr, const Fits::Plio& algo) {
  int status = 0;
  fits_set_compression_type(fptr, PLIO_1, &status);
  CfitsioError::may_throw(status, fptr, "Cannot set compression type to Plio");
  set_tiling(fptr, algo.tiling());
  set_quantize(fptr, algo.quantization());
}

} // namespace ImageCompression
} // namespace Cfitsio
} // namespace Euclid

#endif
