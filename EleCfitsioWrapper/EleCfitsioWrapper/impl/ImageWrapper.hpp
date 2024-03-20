// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_IMAGEWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "Linx/Data/Tiling.h" // rows

#include <iostream> // FIXME rm

namespace Cfitsio {
namespace ImageIo {

/**
 * @brief Variable dimension case.
 */
template <>
Linx::Position<-1> read_shape<-1>(fitsfile* fptr);

template <Linx::Index N>
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

template <Linx::Index N>
void update_shape(fitsfile* fptr, const Linx::Position<N>& shape)
{
  int status = 0;
  int imgtype = 0;
  fits_get_img_type(fptr, &imgtype, &status);
  auto nonconst_shape = shape;
  fits_resize_img(fptr, imgtype, shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot reshape raster.");
}

template <typename T, Linx::Index N>
void update_type_shape(fitsfile* fptr, const Linx::Position<N>& shape)
{
  int status = 0;
  auto nonconst_shape = shape;
  fits_resize_img(fptr, TypeCode<T>::bitpix(), shape.size(), nonconst_shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot reshape raster.");
}

template <typename T, Linx::Index N>
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

template <typename T, Linx::Index M, Linx::Index N>
Linx::Raster<T, M> read_region(fitsfile* fptr, const Linx::Box<N>& region)
{
  Linx::Raster<T, M> raster(Linx::slice<M>(region.shape()));
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

template <Linx::Index N, typename TIn>
void write_region(fitsfile* fptr, const Linx::Box<N>& region, TIn& in)
{
  int status = 0;
  auto step = region.step();
  auto row_fronts = project(region);
  ++row_fronts; // 1-based

  const auto size = region.length(0);
  std::vector<std::decay_t<typename TIn::Value>> nonconst_data(size);
  for (auto p : row_fronts) {
    std::copy_n(&in[p - row_fronts.front()], size, nonconst_data.data());
    fits_write_pix(fptr, TypeCode<typename TIn::Value>::for_image(), p.data(), size, nonconst_data.data(), &status);
  }
}

} // namespace ImageIo
} // namespace Cfitsio

#endif
