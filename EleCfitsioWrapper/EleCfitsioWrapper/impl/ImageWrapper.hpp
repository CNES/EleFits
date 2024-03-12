// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "Linx/Data/Tiling.h" // rows

#include <iostream> // FIXME rm

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
Linx::Raster<T, N> read_raster(fitsfile* fptr)
{
  Linx::Raster<T, N> raster(read_shape<N>(fptr));
  read_raster_to(fptr, raster);
  return raster;
}

template <typename TOut>
void read_raster_to(fitsfile* fptr, TOut& out)
{
  read_region_to(fptr, Linx::Box<TOut::Dimension>::from_shape(read_shape<TOut::Dimension>(fptr)), out);
}

template <typename T, long M, long N>
Linx::Raster<T, M> read_region(fitsfile* fptr, const Linx::Box<N>& region)
{
  Linx::Raster<T, M> raster(region.shape().template slice<M>());
  read_region_to(fptr, region, raster);
  return raster;
}

template <Linx::Index N, typename TOut>
void read_region_to(fitsfile* fptr, const Linx::Box<N>& region, TOut& out)
{
  int status = 0;
  auto step = region.step();
  auto row_fronts = project(region);
  ++row_fronts; // 1-based

  for (auto p : row_fronts) {
    fits_read_pix(
        fptr,
        TypeCode<typename TOut::Value>::for_image(),
        p.data(), // Cannot be const
        region.length(0),
        nullptr,
        &out[p - row_fronts.front()],
        nullptr,
        &status);
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
    const typename Linx::Raster<const T, N, TContainer>::ConstTile& subraster,
    const Linx::Position<N>& destination)
{
  /* 1-based, flatten region (beginning of each line) */
  const auto shape = subraster.shape().extend(destination);
  auto front = destination + 1;
  auto back = destination + shape;
  back[0] = front[0];

  /* Screening positions */
  const auto dst_size = shape[0];
  const auto delta = extend(subraster.region().front(), front) - front;

  /* Process each line */
  int status = 0;
  Linx::Position<N> dst_back;
  Linx::Position<N> src_front;
  std::vector<std::decay_t<T>> line(dst_size);
  Linx::Box<N> dst_region {LINX_MOVE(front), LINX_MOVE(back)};
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
