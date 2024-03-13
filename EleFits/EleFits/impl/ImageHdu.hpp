// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_IMAGEHDU_IMPL) || defined(CHECK_QUALITY)

#include "EleFits/ImageHdu.h"

namespace Euclid {
namespace Fits {

template <Linx::Index N>
Linx::Position<N> ImageHdu::read_shape() const
{
  return m_raster.read_shape<N>();
}

template <typename T, Linx::Index N>
void ImageHdu::update_type_shape(const Linx::Position<N>& shape) const
{
  return m_raster.update_type_shape<T, N>(shape);
}

template <typename T, Linx::Index N>
Linx::Raster<T, N> ImageHdu::read_raster() const
{
  return m_raster.read<T, N>();
}

template <typename TRaster>
void ImageHdu::write_raster(const TRaster& data) const
{
  m_raster.write(data);
}

/**
 * @brief Specialization of `Hdu::as()` for the data unit.
 */
template <>
const ImageRaster& Hdu::as() const;

} // namespace Fits
} // namespace Euclid

#endif
