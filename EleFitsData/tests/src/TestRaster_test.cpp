// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(TestRaster_test)

//-----------------------------------------------------------------------------

template <typename TRaster>
void checkRasterEqualsItself(const TRaster& raster)
{
  BOOST_TEST(Test::raster_approx(raster, raster));
}

template <typename T>
void checkRandomRasterEqualsItself()
{
  checkRasterEqualsItself(Test::RandomRaster<T, 0>({}));
  checkRasterEqualsItself(Test::RandomRaster<T, 1>({2}));
  checkRasterEqualsItself(Test::RandomRaster<T, 2>({2, 3}));
  checkRasterEqualsItself(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRasterEqualsItself(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_raster_equals_itself_test)
{
  checkRasterEqualsItself(Test::SmallRaster());
}

#define RANDOM_RASTER_EQUALS_ITSELF_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_raster_equals_itself_test) \
  { \
    checkRandomRasterEqualsItself<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_RASTER_EQUALS_ITSELF_TEST)

template <typename TRaster>
void checkRastersWithDifferentShapesDiffer(const TRaster& raster)
{
  Linx::Position<TRaster::Dimension> shape = raster.shape();
  shape[0] = raster.shape()[1];
  shape[1] = raster.shape()[0];
  Linx::PtrRaster<const typename TRaster::Value, TRaster::Dimension> other(
      shape,
      raster.data()); // Same data, different shape
  BOOST_TEST(not Test::raster_approx(other, raster));
}

template <typename T>
void checkRandomRastersWithDifferentShapesDiffer()
{
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 2>({2, 3}));
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRastersWithDifferentShapesDiffer(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_rasters_with_different_shapes_differ_test)
{
  checkRastersWithDifferentShapesDiffer(Test::SmallRaster());
}

#define RANDOM_RASTERS_WITH_DIFFERENT_SHAPES_DIFFER_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_random_rasters_with_different_shapes_differ_test) \
  { \
    checkRandomRastersWithDifferentShapesDiffer<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RANDOM_RASTERS_WITH_DIFFERENT_SHAPES_DIFFER_TEST)

template <typename T, long N>
void checkRastersWithDifferentValuesDiffer(const Linx::Raster<T, N>& raster)
{
  Linx::Raster<T, N> other(raster.shape());
  BOOST_TEST(not Test::raster_approx(other, raster));
  BOOST_TEST(not Test::raster_approx(raster, other));
}

template <typename T>
void checkRandomRastersWithDifferentValuesDiffer()
{
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 1>({2}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 2>({2, 3}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 3>({2, 3, 4}));
  checkRastersWithDifferentValuesDiffer(Test::RandomRaster<T, 4>({2, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(small_rasters_with_different_values_differ_test)
{
  checkRastersWithDifferentValuesDiffer(Test::SmallRaster());
}

#define RASTERS_WITH_DIFFERENT_VALUES_DIFFER_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_rasters_with_different_values_differ_test) \
  { \
    checkRandomRastersWithDifferentValuesDiffer<type>(); \
  }

ELEFITS_FOREACH_RASTER_TYPE(RASTERS_WITH_DIFFERENT_VALUES_DIFFER_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
