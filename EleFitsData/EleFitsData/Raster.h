// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_RASTER_H
#define _ELEFITSDATA_RASTER_H

#include "Linx/Data/Raster.h"

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @ingroup image_data_classes
 * @brief Loop over supported raster types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EleFitsPrintSupportedTypes to display all supported types
 * @see ELEFITS_FOREACH_RECORD_TYPE
 * @see ELEFITS_FOREACH_COLUMN_TYPE
 */
#define ELEFITS_FOREACH_RASTER_TYPE(MACRO) \
  MACRO(char, char) \
  MACRO(std::int16_t, int16) \
  MACRO(std::int32_t, int32) \
  MACRO(std::int64_t, int64) \
  MACRO(float, float) \
  MACRO(double, double) \
  MACRO(unsigned char, uchar) \
  MACRO(std::uint16_t, uint16) \
  MACRO(std::uint32_t, uint32) \
  MACRO(std::uint64_t, uint64)

/**
 * @relates Raster
 * @brief Get the `BITPIX` value of a given raster.
 */
template <typename TRaster>
constexpr long bitpix(const TRaster&)
{ // FIXME move to ImageHdu?
  return bitpix<std::decay_t<typename TRaster::Value>>();
}

} // namespace Fits
} // namespace Euclid

#endif
