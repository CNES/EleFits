/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <boost/test/unit_test.hpp>

#include "EL_FitsData/TestRaster.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(TestRaster_test)

//-----------------------------------------------------------------------------

template <typename T, long n>
void checkSelfApprox(const Raster<T, n> &raster) {
  BOOST_CHECK(Test::rasterApprox(raster, raster));
}

template <typename T>
void checkSelfApproxRandom() {
  checkSelfApprox(Test::RandomRaster<T, 0>({}));
  checkSelfApprox(Test::RandomRaster<T, 1>({ 2 }));
  checkSelfApprox(Test::RandomRaster<T, 2>({ 2, 3 }));
  checkSelfApprox(Test::RandomRaster<T, 3>({ 2, 3, 4 }));
  checkSelfApprox(Test::RandomRaster<T, 4>({ 2, 3, 4, 5 }));
}

BOOST_AUTO_TEST_CASE(approx_self_test) {
  checkSelfApprox(Test::SmallRaster());
  checkSelfApproxRandom<char>();
  checkSelfApproxRandom<std::int16_t>();
  checkSelfApproxRandom<std::int32_t>();
  checkSelfApproxRandom<std::int64_t>();
  checkSelfApproxRandom<unsigned char>();
  checkSelfApproxRandom<std::uint16_t>();
  checkSelfApproxRandom<std::uint32_t>();
  checkSelfApproxRandom<std::uint64_t>();
  checkSelfApproxRandom<float>();
  checkSelfApproxRandom<double>();
}

template <typename T, long n>
void checkDifferentShapesNotApprox(const Raster<T, n> &raster) {
  auto shape = raster.shape;
  shape[0]++;
  VecRaster<T, n> other(shape);
  for (long i = 0; i < raster.size(); ++i) {
    other.vector()[i] = *(raster.data() + i);
  }
  BOOST_CHECK(not Test::rasterApprox(other, raster));
}

template <typename T>
void checkDifferentShapesNotApproxRandom() {
  checkDifferentShapesNotApprox(Test::RandomRaster<T, 1>({ 2 }));
  checkDifferentShapesNotApprox(Test::RandomRaster<T, 2>({ 2, 3 }));
  checkDifferentShapesNotApprox(Test::RandomRaster<T, 3>({ 2, 3, 4 }));
  checkDifferentShapesNotApprox(Test::RandomRaster<T, 4>({ 2, 3, 4, 5 }));
}

BOOST_AUTO_TEST_CASE(different_shapes_not_approx_test) {
  checkDifferentShapesNotApprox(Test::SmallRaster());
  checkDifferentShapesNotApproxRandom<char>();
  checkDifferentShapesNotApproxRandom<std::int16_t>();
  checkDifferentShapesNotApproxRandom<std::int32_t>();
  checkDifferentShapesNotApproxRandom<std::int64_t>();
  checkDifferentShapesNotApproxRandom<unsigned char>();
  checkDifferentShapesNotApproxRandom<std::uint16_t>();
  checkDifferentShapesNotApproxRandom<std::uint32_t>();
  checkDifferentShapesNotApproxRandom<std::uint64_t>();
  checkDifferentShapesNotApproxRandom<float>();
  checkDifferentShapesNotApproxRandom<double>();
}

template <typename T, long n>
void checkDifferentValuesNotApprox(const VecRaster<T, n> &raster) {
  VecRaster<T, n> other(raster.shape);
  BOOST_CHECK(not Test::rasterApprox(other, raster));
  BOOST_CHECK(not Test::rasterApprox(raster, other));
}

template <typename T>
void checkDifferentValuesNotApproxRandom() {
  checkDifferentValuesNotApprox(Test::RandomRaster<T, 1>({ 2 }));
  checkDifferentValuesNotApprox(Test::RandomRaster<T, 2>({ 2, 3 }));
  checkDifferentValuesNotApprox(Test::RandomRaster<T, 3>({ 2, 3, 4 }));
  checkDifferentValuesNotApprox(Test::RandomRaster<T, 4>({ 2, 3, 4, 5 }));
}

BOOST_AUTO_TEST_CASE(not_approx_small_test) {
  checkDifferentValuesNotApprox(Test::SmallRaster());
}

#define NOT_APPROX_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(not_approx_##name##_test) { \
    checkDifferentValuesNotApproxRandom<type>(); \
  }

EL_FITSIO_FOREACH_RASTER_TYPE(NOT_APPROX_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
