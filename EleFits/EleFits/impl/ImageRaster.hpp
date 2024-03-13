// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_IMAGERASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/ImageRaster.h"

namespace Euclid {
namespace Fits {

template <Linx::Index N>
Linx::Position<N> ImageRaster::read_shape() const
{
  m_edit();
  return Cfitsio::ImageIo::read_shape<N>(m_fptr);
}

template <Linx::Index N>
void ImageRaster::update_shape(const Linx::Position<N>& shape) const
{
  m_edit();
  Cfitsio::ImageIo::update_shape<N>(m_fptr, shape);
}

template <typename T, Linx::Index N>
void ImageRaster::update_type_shape(const Linx::Position<N>& shape) const
{
  m_edit();
  Cfitsio::ImageIo::update_type_shape<T, N>(m_fptr, shape);
}

template <typename T, Linx::Index N>
Linx::Raster<T, N> ImageRaster::read() const
{
  Linx::Raster<T, N> raster(read_shape<N>());
  read_to(raster);
  return raster;
}

template <typename TRaster>
void ImageRaster::read_to(TRaster& raster) const
{
  m_touch();
  Cfitsio::ImageIo::read_raster_to(m_fptr, raster);
}

template <typename T, Linx::Index M, Linx::Index N>
Linx::Raster<T, M> ImageRaster::read_region(const Linx::Box<N>& region) const
{
  Linx::Raster<T, M> raster(region.shape().template slice<M>());
  read_region_to(region.front(), raster);
  return raster;
}

template <typename TRaster>
void ImageRaster::read_region_to(FileMemRegions<TRaster::Dimension> regions, TRaster& raster) const
{
  const auto& mem_region = regions.inMemory();
  if (raster.isContiguous(mem_region)) {
    read_region_to_slice(regions.inFile().front(), raster.slice(mem_region));
  } else {
    read_region_to_subraster(regions.inFile().front(), raster.subraster(mem_region));
  }
}

template <typename T, Linx::Index N, typename TContainer>
void ImageRaster::read_region_to(typename Linx::Raster<T, N, TContainer>::Tile<N>& subraster) const
{
  read_region_to_subraster(subraster.region().front(), subraster);
}

template <typename TRaster, Linx::Index N>
void ImageRaster::read_region_to_slice(const Linx::Position<N>& front_position, TRaster& raster) const
{
  m_touch();
  Cfitsio::ImageIo::read_region_to(
      m_fptr,
      Linx::Box<N>::from_shape(front_position, raster.shape()), // FIXME use front_position in ImageIo
      raster);
}

template <typename T, Linx::Index M, Linx::Index N, typename TContainer>
void ImageRaster::read_region_to_subraster(
    const Linx::Position<N>& front_position,
    typename Linx::Raster<T, M, TContainer>::Tile<M>& subraster) const
{
  m_touch();
  Cfitsio::ImageIo::read_region_to(
      m_fptr,
      Linx::Box<N>::from_shape(front_position, subraster.shape()), // FIXME use front_position in ImageIo
      subraster);
  // FIXME move algo here, rely solely on read_region_to(fitsfile, Linx::Position, Raster)
}

template <typename TRaster>
void ImageRaster::write(const TRaster& raster) const
{
  m_edit();
  Cfitsio::ImageIo::write_raster(m_fptr, raster);
}

template <typename TRaster, Linx::Index N>
void ImageRaster::write_region(FileMemRegions<N> regions, const TRaster& raster) const
{
  regions.resolve(read_shape<N>() - 1, raster.shape() - 1);
  if (raster.is_contiguous(regions.memory())) {
    write_slice(regions.file().front(), raster.slice(regions.memory()));
  } else {
    write_subraster(regions.file().front(), raster(regions.memory()));
  }
}

template <typename TRaster, Linx::Index N>
void ImageRaster::write_slice(const Linx::Position<N>& front_position, const TRaster& raster) const
{
  Cfitsio::ImageIo::write_region(m_fptr, raster, front_position);
}

template <Linx::Index N, typename TPatch>
void ImageRaster::write_subraster(const Linx::Position<N>& front_position, const TPatch& subraster) const
{
  m_edit();
  int status = 0;
  auto front = Linx::Position<TPatch::Dimension>::zero();
  auto back = subraster.domain().shape() - 1;
  back[0] = 0;
  Linx::Box<TPatch::Dimension> locus {front, back};
  const auto nelem = subraster.domain().length(0);
  const auto delta = front_position.template slice<TPatch::Dimension>();
  Linx::Position<N> target;
  for (const auto& source : locus) {
    target = (source + delta).extend(front_position) + 1; // 1-based
    const auto b = &subraster[source];
    const auto e = b + nelem;
    std::vector<std::decay_t<typename TPatch::Value>> nonconst_data(b, e);
    fits_write_pix(
        m_fptr,
        Cfitsio::TypeCode<typename TPatch::Value>::for_image(),
        target.data(),
        nelem,
        nonconst_data.data(),
        &status);
    // TODO to ImageWrapper
  }
}

} // namespace Fits
} // namespace Euclid

#endif
