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

#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(TestRaster_test)

//-----------------------------------------------------------------------------

template <typename T, long N, typename TContainer> // FIXME simplify
void checkRasterEqualsItself(const Raster<T, N, TContainer>& raster) {
  BOOST_TEST(Test::rasterApprox(raster, raster));
}

template <typename T>
void checkRandomRasterEqualsItself() {
  checkRasterEqualsItself(Test::RandomRaster<T, 0>({}));
  checkRasterEqualsItself(Test::RandomRaster<T, 1>({2}));
  checkRasterEqualsItself(Test::RandomRaster<T, 2>({2, 3}));
  checkRasterEqualsItself(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRasterEqualsItself(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_raster_equals_itself_test) {
  checkRasterEqualsItself(Test::SmallRaster());
}

#define RANDOM_RASTER_EQUALS_ITSELF_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_raster_equals_itself_test) { \
    checkRandomRasterEqualsItself<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_RASTER_EQUALS_ITSELF_TEST)

template <typename T, long N, typename TContainer> // FIXME simplify
void checkRastersWithDifferentShapesDiffer(const Raster<T, N, TContainer>& raster) {
  auto shape = raster.shape();
  shape[0]++;
  VecRaster<T, N> other(shape);
  for (long i = 0; i < raster.size(); ++i) {
    *(other.data() + i) = *(raster.data() + i);
  }
  BOOST_TEST(not Test::rasterApprox(other, raster));
}

template <typename T>
void checkRandomRastersWithDifferentShapesDiffer() {
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 1>({2}));
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 2>({2, 3}));
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_rasters_with_different_shapes_differ_test) {
  checkRastersWithDifferentShapesDiffer(Test::SmallRaster());
}

#define RANDOM_RASTERS_WITH_DIFFERENT_SHAPES_DIFFER_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_rasters_with_different_shapes_differ_test) { \
    checkRandomRastersWithDifferentShapesDiffer<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_RASTERS_WITH_DIFFERENT_SHAPES_DIFFER_TEST)

template <typename T, long N>
void checkRastersWithDifferentValuesDiffer(const VecRaster<T, N>& raster) {
  VecRaster<T, N> other(raster.shape());
  BOOST_TEST(not Test::rasterApprox(other, raster));
  BOOST_TEST(not Test::rasterApprox(raster, other));
}

template <typename T>
void checkRandomRastersWithDifferentValuesDiffer() {
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 1>({2}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 2>({2, 3}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_rasters_with_different_values_differ_test) {
  checkRastersWithDifferentValuesDiffer(Test::SmallRaster());
}

#define RASTERS_WITH_DIFFERENT_VALUES_DIFFER_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_rasters_with_different_values_differ_test) { \
    checkRandomRastersWithDifferentValuesDiffer<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RASTERS_WITH_DIFFERENT_VALUES_DIFFER_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
