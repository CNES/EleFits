// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRaster.h"

namespace Fits {
namespace Test {

SmallRaster::SmallRaster(Linx::Index rasterWidth, Linx::Index rasterHeight) :
    Linx::Raster<float>({rasterWidth, rasterHeight}), width(rasterWidth), height(rasterHeight)
{
  for (Linx::Index y = 0; y < shape()[1]; ++y) {
    for (Linx::Index x = 0; x < shape()[0]; ++x) {
      operator[]({x, y}) = 0.1F * float(y) + float(x);
    }
  }
}

} // namespace Test
} // namespace Fits
