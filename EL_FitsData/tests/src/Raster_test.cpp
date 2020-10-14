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
#include "EL_FitsData/Raster.h"

using namespace Euclid;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Raster_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(index_test) {

  /* Fixed dimension */
  FitsIO::Position<4> fixedShape;
  for (auto &length : fixedShape) {
    length = std::rand();
  }
  FitsIO::Position<4> fixedPos;
  for (auto &coord : fixedPos) {
    coord = std::rand();
  }
  auto fixedIndex = FitsIO::Internal::IndexRecursionImpl<4>::index(fixedShape, fixedPos);
  BOOST_CHECK_EQUAL(
      fixedIndex,
      fixedPos[0] + fixedShape[0] * (fixedPos[1] + fixedShape[1] * (fixedPos[2] + fixedShape[2] * (fixedPos[3]))));

  /* Variable dimension */
  FitsIO::Position<-1> variableShape(fixedShape.begin(), fixedShape.end());
  FitsIO::Position<-1> variablePos(fixedPos.begin(), fixedPos.end());
  auto variableIndex = FitsIO::Internal::IndexRecursionImpl<-1>::index(variableShape, variablePos);
  BOOST_CHECK_EQUAL(variableIndex, fixedIndex);
}

BOOST_FIXTURE_TEST_CASE(small_raster_size_test, FitsIO::Test::SmallRaster) {
  long size(this->width * this->height);
  BOOST_CHECK_EQUAL(this->dimension(), 2);
  BOOST_CHECK_EQUAL(this->size(), size);
  BOOST_CHECK_EQUAL(this->vector().size(), size);
}

BOOST_AUTO_TEST_CASE(variable_dimension_raster_size_test) {
  const long width = 4;
  const long height = 3;
  const long size = width * height;
  FitsIO::Test::RandomRaster<int, -1> raster({ width, height });
  BOOST_CHECK_EQUAL(raster.dimension(), 2);
  BOOST_CHECK_EQUAL(raster.size(), size);
  BOOST_CHECK_EQUAL(raster.vector().size(), size);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
