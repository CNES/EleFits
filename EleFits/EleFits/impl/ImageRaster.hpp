// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_IMAGERASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleFits/ImageRaster.h"
#include "Linx/Data/Box.h"

namespace Fits {

template <Linx::Index N>
Linx::Position<N> ImageRaster::read_shape() const
{
  m_edit();
  return Cfitsio::ImageIo::read_shape<N>(m_fptr);
}

template <Linx::Index N>
void ImageRaster::update_shape(Linx::Position<N> shape) const
{
  m_edit();
  Cfitsio::ImageIo::update_shape<N>(m_fptr, LINX_MOVE(shape));
}

template <typename T, Linx::Index N>
void ImageRaster::update_type_shape(Linx::Position<N> shape) const
{
  m_edit();
  Cfitsio::ImageIo::update_type_shape<T, N>(m_fptr, LINX_MOVE(shape));
}

template <typename T, Linx::Index N>
Linx::Raster<T, N> ImageRaster::read() const
{
  Linx::Raster<T, N> raster(read_shape<N>());
  read_to(raster);
  return raster;
}

template <typename TOut>
void ImageRaster::read_to(TOut& out) const
{
  m_touch();
  Cfitsio::ImageIo::read_raster_to(m_fptr, out);
}

template <typename T, Linx::Index M, Linx::Index N>
Linx::Raster<T, M> ImageRaster::read_region(const Linx::Box<N>& region) const
{
  Linx::Raster<T, M> raster(Linx::slice<M>(region.shape()));
  read_region_to(region.front(), raster);
  return raster;
}

template <Linx::Index N, typename TOut>
void ImageRaster::read_region_to(Linx::Position<N> front, TOut& out) const
{
  m_touch();
  Cfitsio::ImageIo::read_region_to(
      m_fptr,
      Linx::Box<N>::from_shape(LINX_MOVE(front), out.domain().shape()),
      out); // FIXME give only front
}

template <typename TIn>
void ImageRaster::write(const TIn& in) const
{
  write_region(Linx::Position<TIn::Dimension>::zero(in.domain().dimension()), in); // FIXME Add Patch::dimension()
  // FIXME Cfitsio::ImageIo::write_raster() for performance?
}

template <Linx::Index N, typename TIn>
void ImageRaster::write_region(Linx::Position<N> front, const TIn& in) const
{
  m_edit();
  Cfitsio::ImageIo::write_region(m_fptr, Linx::Box<N>::from_shape(LINX_MOVE(front), in.domain().shape()), in);
}

} // namespace Fits

#endif
