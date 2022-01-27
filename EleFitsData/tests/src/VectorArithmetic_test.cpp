/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFitsData/TestRaster.h"
#include "EleFitsData/VectorArithmetic.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(VectorArithmetic_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(raster_arithmetic_test) {
  const Test::SmallRaster lhs;
  const Test::SmallRaster rhs;
  int scalar = 2;
  const auto plusVector = lhs + rhs;
  const auto plusScalar = lhs + scalar;
  const auto minusVector = lhs - rhs;
  const auto minusScalar = lhs - scalar;
  const auto timesScalar = lhs * scalar;
  const auto dividedByScalar = lhs / scalar;
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    BOOST_TEST(plusVector[i] == lhs[i] + rhs[i]);
    BOOST_TEST(plusScalar[i] == lhs[i] + scalar);
    BOOST_TEST(minusVector[i] == lhs[i] - rhs[i]);
    BOOST_TEST(minusScalar[i] == lhs[i] - scalar);
    BOOST_TEST(timesScalar[i] == lhs[i] * scalar);
    BOOST_TEST(dividedByScalar[i] == lhs[i] / scalar);
  }
}

BOOST_AUTO_TEST_CASE(raster_generate_test) {
  Position<3> shape {3, 14, 15};
  Test::RandomRaster<std::int16_t, 3> a(shape);
  Test::RandomRaster<std::int32_t, 3> b(shape);
  VecRaster<std::int64_t, 3> result(shape);
  result.generate(
      [](auto v, auto w) {
        return v * w;
      },
      a,
      b);
  result.apply([](auto v) {
    return -v;
  });
  for (const auto& p : result.domain()) {
    BOOST_TEST((result[p] == -a[p] * b[p]));
  }
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
