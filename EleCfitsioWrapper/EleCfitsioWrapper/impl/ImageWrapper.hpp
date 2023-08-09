// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace ImageIo {

/**
 * @brief Variable dimension case.
 */
template <>
Fits::Position<-1> readShape<-1>(fitsfile* fptr);

template <long N>
Fits::Position<N> readShape(fitsfile* fptr) {
  Fits::Position<N> shape;
  int status = 0;
  fits_get_img_size(fptr, N, &shape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read raster shape.");
  return shape;
}

bool is_compressed(fitsfile* fptr) {
  int status = 0;
  bool is_compressed = fits_is_compressed_image(fptr, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot determine if image is compressed.");
  return is_compressed;
}

template <long N>
void updateShape(fitsfile* fptr, const Fits::Position<N>& shape) {
  int status = 0;
  int imgtype = 0;
  fits_get_img_type(fptr, &imgtype, &status);
  auto nonconstShape = shape;
  fits_resize_img(fptr, imgtype, shape.size(), nonconstShape.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot reshape raster.");
}

template <typename T, long N>
void updateTypeShape(fitsfile* fptr, const Fits::Position<N>& shape) {
  int status = 0;
  auto nonconstShape = shape;
  fits_resize_img(fptr, TypeCode<T>::bitpix(), shape.size(), nonconstShape.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot reshape raster.");
}

template <typename T, long N>
Fits::VecRaster<T, N> readRaster(fitsfile* fptr) {
  Fits::VecRaster<T, N> raster(readShape<N>(fptr));
  readRasterTo(fptr, raster);
  return raster;
}

template <typename TRaster>
void readRasterTo(fitsfile* fptr, TRaster& destination) {
  int status = 0;
  const auto size = destination.size();
  fits_read_img(
      fptr,
      TypeCode<std::decay_t<typename TRaster::Value>>::forImage(),
      1, // Number 1 is a 1-based index (so we read the whole raster here)
      size,
      nullptr,
      destination.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read raster.");
}

template <typename T, long N, typename TContainer>
void readRasterTo(fitsfile* fptr, Fits::Subraster<T, N, TContainer>& destination) {
  const auto region = Fits::Region<N>::fromShape(Fits::Position<N>::zero(), readShape<N>(fptr));
  readRegionTo(fptr, region, destination);
}

template <typename T, long M, long N>
Fits::VecRaster<T, M> readRegion(fitsfile* fptr, const Fits::Region<N>& region) {
  Fits::VecRaster<T, M> raster(region.shape().template slice<M>());
  readRegionTo(fptr, region, raster);
  return raster;
}

template <typename TRaster, long N>
void readRegionTo(fitsfile* fptr, const Fits::Region<N>& region, TRaster& raster) {
  int status = 0;
  const std::size_t dim = region.dimension();
  Fits::Position<N> front = region.front; // Copy for const-correctness
  Fits::Position<N> back = region.back; // idem
  std::vector<long> step(dim, 1);
  for (std::size_t i = 0; i < dim; ++i) {
    front[i]++; // CFITSIO is 1-based
    back[i]++; // idem
  }
  fits_read_subset(
      fptr,
      TypeCode<std::decay_t<typename TRaster::Value>>::forImage(),
      front.data(),
      back.data(),
      step.data(),
      nullptr,
      raster.data(),
      nullptr,
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read image region.");
}

template <typename T, long M, long N, typename TContainer>
void readRegionTo(fitsfile* fptr, const Fits::Region<N>& region, Fits::Subraster<T, M, TContainer>& destination) {

  /* 1-based, flatten region (beginning of each line) */
  Fits::Region<N> srcRegion = region + 1;
  srcRegion.back[0] = srcRegion.front[0];
  const auto srcCount = srcRegion.size();

  /* Screening positions */
  auto srcFront = srcRegion.front;
  auto srcBack = srcFront;
  srcBack[0] += region.shape()[0] - 1;
  auto dstFront = destination.region().front;
  Fits::RegionScreener<N> srcScreener(srcRegion, {srcBack, dstFront});

  /* Step */
  std::vector<long> step(srcRegion.dimension(), 1L);

  /* Process each line */
  int status = 0;
  for (long i = 0; i < srcCount; ++i) {
    fits_read_subset(
        fptr,
        TypeCode<T>::forImage(),
        srcFront.data(),
        srcBack.data(),
        step.data(),
        nullptr,
        &destination.parent()[dstFront],
        nullptr,
        &status);
    CfitsioError::mayThrow(status, fptr, "Cannot read image region.");
    srcScreener.next();
    srcFront = srcScreener.current();
    srcBack = srcScreener.followers()[0];
    dstFront = srcScreener.followers()[1];
  }
}

template <typename TRaster>
void writeRaster(fitsfile* fptr, const TRaster& raster) {
  mayThrowReadonlyError(fptr);
  int status = 0;
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  using Value = std::decay_t<typename TRaster::Value>;
  std::vector<Value> nonconstData(begin, end); // For const-correctness issue
  fits_write_img(fptr, TypeCode<Value>::forImage(), 1, raster.size(), nonconstData.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write image.");
}

template <typename TRaster, long N>
void writeRegion(fitsfile* fptr, const TRaster& raster, const Fits::Position<N>& destination) {
  int status = 0;
  auto front = destination + 1;
  const auto shape = raster.shape().extend(destination);
  auto back = destination + shape; // = front + raster.shape() - 1
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  using Value = std::decay_t<typename TRaster::Value>;
  std::vector<Value> nonconstData(begin, end); // For const-correctness issue
  fits_write_subset(fptr, TypeCode<Value>::forImage(), front.data(), back.data(), nonconstData.data(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write image region.");
}

template <typename T, long M, long N, typename TContainer>
void writeRegion(
    fitsfile* fptr,
    const Fits::Subraster<T, M, TContainer>& subraster,
    const Fits::Position<N>& destination) {

  /* 1-based, flatten region (beginning of each line) */
  const auto shape = subraster.shape().extend(destination);
  Fits::Region<N> dstRegion {destination + 1, destination + shape};
  dstRegion.back[0] = dstRegion.front[0];

  /* Screening positions */
  const auto dstSize = shape[0];
  const auto delta = subraster.region().front.extend(dstRegion.front) - dstRegion.front;

  /* Process each line */
  int status = 0;
  Fits::Position<N> dstBack;
  Fits::Position<N> srcFront;
  std::vector<std::decay_t<T>> line(dstSize);
  for (auto dstFront : dstRegion) {
    dstBack = dstFront;
    dstBack[0] += dstSize - 1;
    srcFront = dstFront + delta;
    line.assign(&subraster[srcFront], &subraster[srcFront] + dstSize);
    fits_write_pix(fptr, TypeCode<T>::forImage(), dstFront.data(), dstSize, line.data(), &status);
    // fits_write_subset(fptr, TypeCode<T>::forImage(), dstFront.data(), dstBack.data(), line.data(), &status);
    CfitsioError::mayThrow(status, fptr, "Cannot write image region.");
  }
}

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

#endif
