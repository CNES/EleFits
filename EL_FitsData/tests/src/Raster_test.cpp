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

#include "EL_FitsData/Raster.h"
#include "EL_FitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Raster_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(index_test) {

  /* Fixed dimension */
  Position<4> fixedShape;
  for (auto& length : fixedShape) {
    length = std::rand();
  }
  Position<4> fixedPos;
  for (auto& coord : fixedPos) {
    coord = std::rand();
  }
  auto fixedIndex = Internal::IndexRecursionImpl<4>::index(fixedShape, fixedPos);
  BOOST_TEST(
      fixedIndex ==
      fixedPos[0] + fixedShape[0] * (fixedPos[1] + fixedShape[1] * (fixedPos[2] + fixedShape[2] * (fixedPos[3]))));

  /* Variable dimension */
  Position<-1> variableShape(fixedShape.begin(), fixedShape.end());
  Position<-1> variablePos(fixedPos.begin(), fixedPos.end());
  auto variableIndex = Internal::IndexRecursionImpl<-1>::index(variableShape, variablePos);
  BOOST_TEST(variableIndex == fixedIndex);
}

BOOST_AUTO_TEST_CASE(raster_data_test) {
  int data[] = { 0, 1, 2 };
  PtrRaster<int, 1> raster({ 3 }, data);
  BOOST_TEST(raster.data() != nullptr);
  BOOST_TEST(raster[{ 0 }] == 0);
}

BOOST_AUTO_TEST_CASE(const_raster_data_test) {
  const int cData[] = { 3, 4, 5 };
  PtrRaster<const int, 1> cRaster({ 3 }, cData);
  BOOST_TEST(cRaster.data() != nullptr);
  BOOST_TEST(cRaster[{ 0 }] == 3);
}

BOOST_AUTO_TEST_CASE(vec_raster_data_test) {
  VecRaster<int, 1> vecRaster({ 3 });
  BOOST_TEST(vecRaster.data() != nullptr);
  BOOST_TEST(vecRaster[{ 0 }] == 0);
}

BOOST_AUTO_TEST_CASE(const_vec_raster_data_test) {
  const VecRaster<int, 1> cVecRaster({ 3 });
  BOOST_TEST(cVecRaster.data() != nullptr);
  BOOST_TEST(cVecRaster[{ 0 }] == 0);
}

BOOST_FIXTURE_TEST_CASE(small_raster_size_test, Test::SmallRaster) {
  long size(this->width * this->height);
  BOOST_TEST(this->dimension() == 2);
  BOOST_TEST(this->size() == size);
  BOOST_TEST(this->vector().size() == size);
}

BOOST_AUTO_TEST_CASE(variable_dimension_raster_size_test) {
  const long width = 4;
  const long height = 3;
  const long size = width * height;
  Test::RandomRaster<int, -1> raster({ width, height });
  BOOST_TEST(raster.dimension() == 2);
  BOOST_TEST(raster.size() == size);
  BOOST_TEST(raster.vector().size() == size);
}

BOOST_AUTO_TEST_CASE(subscript_bounds_test) {
  const long width = 4;
  const long height = 3;
  Test::RandomRaster<int> raster({ width, height });
  raster.at({ 1, -1 }) = 1;
  BOOST_TEST(raster.at({ 1, -1 }) == 1);
  const auto& vec = raster.vector();
  BOOST_TEST((raster[{ 0, 0 }]) == vec[0]);
  BOOST_TEST(raster.at({ 0, 0 }) == vec[0]);
  BOOST_TEST(raster.at({ -1, 0 }) == vec[width - 1]);
  BOOST_TEST(raster.at({ -width, 0 }) == vec[0]);
  BOOST_TEST(raster.at({ 0, -1 }) == vec[(height - 1) * width]);
  BOOST_TEST(raster.at({ -1, -1 }) == vec[height * width - 1]);
  BOOST_CHECK_THROW(raster.at({ width, 0 }), OutOfBoundsError);
  BOOST_CHECK_THROW(raster.at({ -1 - width, 0 }), OutOfBoundsError);
  BOOST_CHECK_THROW(raster.at({ 0, height }), OutOfBoundsError);
  BOOST_CHECK_THROW(raster.at({ 0, -1 - height }), OutOfBoundsError);
}

BOOST_FIXTURE_TEST_CASE(vecraster_move_test, Test::SmallRaster) {
  const auto copied = this->vector();
  std::vector<Value> moved;
  this->moveTo(moved);
  BOOST_TEST(moved == copied);
  BOOST_TEST(this->vector().size() == 0);
}

BOOST_AUTO_TEST_CASE(make_raster_test) {
  constexpr long width = 16;
  constexpr long height = 9;
  constexpr long depth = 3;
  short data2[width * height] = { 0 };
  const short constData2[width * height] = { 0 };
  float data3[width * height * depth] = { 0 };
  const float constData3[width * height * depth] = { 0 };
  const auto raster2 = makeRaster({ width, height }, data2);
  const auto constRaster2 = makeRaster({ width, height }, constData2);
  const auto raster3 = makeRaster<3>({ width, height, depth }, data3);
  const auto constRaster3 = makeRaster<3>({ width, height, depth }, constData3);
  const auto rasterDyn = makeRaster<-1>({ width, height, depth }, data3);
  const auto constRasterDyn = makeRaster<-1>({ width, height, depth }, constData3);
  BOOST_TEST(raster2.dimension() == 2);
  BOOST_TEST(constRaster2.dimension() == 2);
  BOOST_TEST(raster3.dimension() == 3);
  BOOST_TEST(constRaster3.dimension() == 3);
  BOOST_TEST(rasterDyn.dimension() == 3);
  BOOST_TEST(constRasterDyn.dimension() == 3);
}

BOOST_AUTO_TEST_CASE(slicing_test) {

  Test::RandomRaster<float, 3> raster({ 5, 3, 4 });

  // Several x-y planes
  Region<3> cube { { 0, 0, 1 }, { 4, 2, 2 } };
  BOOST_TEST(raster.isContiguous<3>(cube));
  const auto cubed = raster.slice<3>(cube);
  BOOST_TEST((cubed.shape == Position<3>({ 5, 3, 2 })));
  BOOST_TEST((cubed[{ 0, 0, 0 }] == raster[cube.front]));

  // One full x-y plane
  Region<3> plane { { 0, 0, 1 }, { 4, 2, 1 } };
  BOOST_TEST(raster.isContiguous<2>(plane));
  const auto planed = raster.slice<2>(plane);
  BOOST_TEST((planed.shape == Position<2>({ 5, 3 })));
  BOOST_TEST((planed[{ 0, 0 }] == raster[plane.front]));

  // One partial x-y plane
  Region<3> rectangle { { 0, 1, 1 }, { 4, 2, 1 } };
  BOOST_TEST(raster.isContiguous<2>(rectangle));
  const auto rectangled = raster.slice<2>(rectangle);
  BOOST_TEST((rectangled.shape == Position<2>({ 5, 2 })));
  BOOST_TEST((rectangled[{ 0, 0 }] == raster[rectangle.front]));

  // One partial x line
  Region<3> segment { { 1, 1, 1 }, { 3, 1, 1 } };
  BOOST_TEST(raster.isContiguous<1>(segment));
  const auto segmented = raster.slice<1>(segment);
  BOOST_TEST((segmented.shape == Position<1>({ 3 })));
  BOOST_TEST((segmented[{ 0 }] == raster[segment.front]));

  // Non-contiguous region
  Region<3> bad { { 1, 1, 1 }, { 2, 2, 2 } };
  BOOST_TEST(not raster.isContiguous<3>(bad));
  BOOST_CHECK_THROW(raster.slice<3>(bad), FitsIOError);
}

BOOST_AUTO_TEST_CASE(sectionning_test) {

  const Test::RandomRaster<short, 3> raster3D({ 8, 9, 12 });

  // 3D
  const auto section3D = raster3D.section(3, 5);
  BOOST_TEST((section3D.shape == Position<3> { 8, 9, 3 }));
  for (const auto p : section3D.domain()) {
    BOOST_TEST((section3D[p] == raster3D[p + Position<3> { 0, 0, 3 }]));
  }

  // 2D
  const auto section2D = raster3D.section(3);
  BOOST_TEST(section2D.shape == Position<2>({ 8, 9 }));
  for (const auto p : section2D.domain()) {
    BOOST_TEST((section2D[p] == raster3D[p.extend<3>({ 0, 0, 3 })]));
  }

  // 1D
  const auto section1D = section2D.section(6);
  BOOST_TEST(section1D.shape == Position<1> { 8 });
  for (const auto p : section1D.domain()) {
    BOOST_TEST((section1D[p] == raster3D[p.extend<3>({ 0, 6, 3 })]));
  }

  // 0D
  const auto section0D = section1D.section(2);
  BOOST_TEST(section0D.dimension() == 0);
  BOOST_TEST((*section0D.data() == raster3D[{ 2, 6, 3 }]));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
