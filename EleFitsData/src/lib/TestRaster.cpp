// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRaster.h"

namespace Euclid {
namespace Fits {
namespace Test {

SmallRaster::SmallRaster(long rasterWidth, long rasterHeight) :
    Linx::VecRaster<float>({rasterWidth, rasterHeight}), width(rasterWidth), height(rasterHeight)
{
  for (long y = 0; y < shape()[1]; ++y) {
    for (long x = 0; x < shape()[0]; ++x) {
      operator[]({x, y}) = 0.1F * float(y) + float(x);
    }
  }
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
