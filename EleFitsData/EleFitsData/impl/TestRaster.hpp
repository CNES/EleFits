// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_TESTRASTER_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/TestRaster.h"
#include "EleFitsData/TestUtils.h"

namespace Euclid {
namespace Fits {
namespace Test {

template <typename TRaster>
bool SmallRaster::approx(const TRaster& other, double tol) const {
  return rasterApprox(*this, other, tol);
}

template <typename T, long N>
RandomRaster<T, N>::RandomRaster(Position<N> rasterShape, T min, T max) :
    VecRaster<T, N>(rasterShape, generate_random_vector<T>(shapeSize(rasterShape), min, max)) {}

template <typename T, long N>
template <typename TRaster>
bool RandomRaster<T, N>::approx(const TRaster& other, double tol) const {
  return rasterApprox(*this, other, tol);
}

template <typename TRaster, typename URaster>
bool rasterApprox(const TRaster& test, const URaster& ref, double tol) {
  if (test.shape() != ref.shape()) {
    return false;
  }
  for (std::size_t i = 0; i < test.size(); ++i) {
    if (not approx(test.data()[i], ref.data()[i], tol)) {
      return false;
    }
  }
  return true;
}

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
