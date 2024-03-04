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
void read_raster_to(fitsfile* fptr, typename Linx::Raster<T, N, TContainer>::Tile& destination)
{
  const auto region = Linx::Box<N>::fromShape(Linx::Position<N>::zero(), read_shape<N>(fptr));
  read_region_to(fptr, region, destination);
}

template <typename T, long M, long N>
Linx::VecRaster<T, M> read_region(fitsfile* fptr, const Linx::Box<N>& region)
{
  Linx::VecRaster<T, M> raster(region.shape().template slice<M>());
  read_region_to(fptr, region, raster);
  return raster;
}

template <typename TRaster, long N>
void read_region_to(fitsfile* fptr, const Linx::Box<N>& region, TRaster& raster)
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
void read_region_to(
    fitsfile* fptr,
    const Linx::Box<N>& region,
    typename Linx::Raster<T, N, TContainer>::Tile& destination)
{
  int status = 0;
  std::vector<long> step(region.dimension(), 1L);
  auto delta = region.front() + 1 - destination.domain().front();
  for (const auto& dst : rows(destination)) {
    auto front = dst.front() + delta; // FIXME pre-allocate
    auto back = front; // FIXME idem
    back[0] += region.length(0) - 1;
    fits_read_subset(
        fptr,
        TypeCode<T>::for_image(),
        front.data(),
        back.data(),
        step.data(),
        nullptr,
        &dst[0],
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
  Linx::Box<N> dst_region {destination + 1, destination + shape};
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
