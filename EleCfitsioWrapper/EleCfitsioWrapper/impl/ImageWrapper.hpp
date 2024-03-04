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
Linx::Position<-1> read_shape<-1>(fitsfile* fptr);

template <long N>
Linx::Position<N> read_shape(fitsfile* fptr)
{
  Linx::Position<N> shape;
  int status = 0;
  fits_get_img_size(fptr, N, &shape[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot read raster shape.");
  return shape;
}

bool is_compressed(fitsfile* fptr)
{
  int status = 0;
  bool is_compressed = fits_is_compressed_image(fptr, &status);
  CfitsioError::may_throw(status, fptr, "Cannot determine if image is compressed.");
  return is_compressed;
}

template <long N>
void update_shape(fitsfile* fptr, const Linx::Position<N>& shape)
{
  int status = 0;
  int imgtype = 0;
  fits_get_img_type(fptr, &imgtype, &status);
  auto nonconst_shape = shape;
  fits_resize_img(fptr, imgtype, shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot reshape raster.");
}

template <typename T, long N>
void update_type_shape(fitsfile* fptr, const Linx::Position<N>& shape)
{
  int status = 0;
  auto nonconst_shape = shape;
  fits_resize_img(fptr, TypeCode<T>::bitpix(), shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot reshape raster.");
}

template <typename T, long N>
Linx::VecRaster<T, N> read_raster(fitsfile* fptr)
{
  Linx::VecRaster<T, N> raster(read_shape<N>(fptr));
  read_raster_to(fptr, raster);
  return raster;
}

template <typename TRaster>
void read_raster_to(fitsfile* fptr, TRaster& destination)
{
  int status = 0;
  const auto size = destination.size();
  fits_read_img(
      fptr,
      TypeCode<std::decay_t<typename TRaster::Value>>::for_image(),
      1, // Number 1 is a 1-based index (so we read the whole raster here)
      size,
      nullptr,
      destination.data(),
      nullptr,
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot read raster.");
}

template <typename T, long N, typename TContainer>
void read_raster_to(fitsfile* fptr, Fits::Subraster<T, N, TContainer>& destination)
{
  const auto region = Fits::Region<N>::fromShape(Linx::Position<N>::zero(), read_shape<N>(fptr));
  read_region_to(fptr, region, destination);
}

template <typename T, long M, long N>
Linx::VecRaster<T, M> read_region(fitsfile* fptr, const Fits::Region<N>& region)
{
  Linx::VecRaster<T, M> raster(region.shape().template slice<M>());
  read_region_to(fptr, region, raster);
  return raster;
}

template <typename TRaster, long N>
void read_region_to(fitsfile* fptr, const Fits::Region<N>& region, TRaster& raster)
{
  int status = 0;
  const std::size_t dim = region.dimension();
  Linx::Position<N> front = region.front; // Copy for const-correctness
  Linx::Position<N> back = region.back; // idem
  std::vector<long> step(dim, 1);
  for (std::size_t i = 0; i < dim; ++i) {
    front[i]++; // CFITSIO is 1-based
    back[i]++; // idem
  }
  fits_read_subset(
      fptr,
      TypeCode<std::decay_t<typename TRaster::Value>>::for_image(),
      front.data(),
      back.data(),
      step.data(),
      nullptr,
      raster.data(),
      nullptr,
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot read image region.");
}

template <typename T, long M, long N, typename TContainer>
void read_region_to(fitsfile* fptr, const Fits::Region<N>& region, Fits::Subraster<T, M, TContainer>& destination)
{
  /* 1-based, flatten region (beginning of each line) */
  Fits::Region<N> src_region = region + 1;
  src_region.back[0] = src_region.front[0];
  const auto src_count = src_region.size();

  /* Screening positions */
  auto src_front = src_region.front;
  auto src_back = src_front;
  src_back[0] += region.shape()[0] - 1;
  auto dst_front = destination.region().front;
  Fits::RegionScreener<N> src_screener(src_region, {src_back, dst_front});

  /* Step */
  std::vector<long> step(src_region.dimension(), 1L);

  /* Process each line */
  int status = 0;
  for (long i = 0; i < src_count; ++i) {
    fits_read_subset(
        fptr,
        TypeCode<T>::for_image(),
        src_front.data(),
        src_back.data(),
        step.data(),
        nullptr,
        &destination.parent()[dst_front],
        nullptr,
        &status);
    CfitsioError::may_throw(status, fptr, "Cannot read image region.");
    src_screener.next();
    src_front = src_screener.current();
    src_back = src_screener.followers()[0];
    dst_front = src_screener.followers()[1];
  }
}

template <typename TRaster>
void write_raster(fitsfile* fptr, const TRaster& raster)
{
  may_throw_readonly(fptr);
  int status = 0;
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  using Value = std::decay_t<typename TRaster::Value>;
  std::vector<Value> nonconst_data(begin, end); // For const-correctness issue
  fits_write_img(fptr, TypeCode<Value>::for_image(), 1, raster.size(), nonconst_data.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot write image.");
}

template <typename TRaster, long N>
void write_region(fitsfile* fptr, const TRaster& raster, const Linx::Position<N>& destination)
{
  int status = 0;
  auto front = destination + 1;
  const auto shape = raster.shape().extend(destination);
  auto back = destination + shape; // = front + raster.shape() - 1
  const auto begin = raster.data();
  const auto end = begin + raster.size();
  using Value = std::decay_t<typename TRaster::Value>;
  std::vector<Value> nonconst_data(begin, end); // For const-correctness issue
  fits_write_subset(fptr, TypeCode<Value>::for_image(), front.data(), back.data(), nonconst_data.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot write image region.");
}

template <typename T, long M, long N, typename TContainer>
void write_region(
    fitsfile* fptr,
    const Fits::Subraster<T, M, TContainer>& subraster,
    const Linx::Position<N>& destination)
{
  /* 1-based, flatten region (beginning of each line) */
  const auto shape = subraster.shape().extend(destination);
  Fits::Region<N> dst_region {destination + 1, destination + shape};
  dst_region.back[0] = dst_region.front[0];

  /* Screening positions */
  const auto dst_size = shape[0];
  const auto delta = subraster.region().front.extend(dst_region.front) - dst_region.front;

  /* Process each line */
  int status = 0;
  Linx::Position<N> dst_back;
  Linx::Position<N> src_front;
  std::vector<std::decay_t<T>> line(dst_size);
  for (auto dst_front : dst_region) {
    dst_back = dst_front;
    dst_back[0] += dst_size - 1;
    src_front = dst_front + delta;
    line.assign(&subraster[src_front], &subraster[src_front] + dst_size);
    fits_write_pix(fptr, TypeCode<T>::for_image(), dst_front.data(), dst_size, line.data(), &status);
    // fits_write_subset(fptr, TypeCode<T>::for_image(), dst_front.data(), dst_back.data(), line.data(), &status);
    CfitsioError::may_throw(status, fptr, "Cannot write image region.");
  }
}

} // namespace ImageIo
} // namespace Cfitsio
} // namespace Euclid

#endif
