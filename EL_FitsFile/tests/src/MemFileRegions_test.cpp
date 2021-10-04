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

#include "EL_FitsFile/MemFileRegions.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(MemFileRegions_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctor_test) {
  const Position<2> memory { 3, 4 };
  const Position<2> file { 6, 8 };
  const Position<2> shape { 7, 9 };

  const MemFileRegions<2> region_position(Region<2>::fromShape(memory, shape), file);
  BOOST_TEST(region_position.inFile().shape() == shape);
  BOOST_TEST(region_position.inFile().front == file);
  BOOST_TEST(region_position.inMemory().shape() == shape);
  BOOST_TEST(region_position.inMemory().front == memory);

  const MemFileRegions<2> position_region(memory, Region<2>::fromShape(file, shape));
  BOOST_TEST(position_region.inFile().shape() == shape);
  BOOST_TEST(position_region.inFile().front == file);
  BOOST_TEST(position_region.inMemory().shape() == shape);
  BOOST_TEST(position_region.inMemory().front == memory);

  const MemFileRegions<2> region(Region<2>::fromShape(file, shape));
  BOOST_TEST(region.inFile().shape() == shape);
  BOOST_TEST(region.inFile().front == file);
  BOOST_TEST(region.inMemory().shape() == shape);
  BOOST_TEST(region.inMemory().front == file);
}

BOOST_AUTO_TEST_CASE(shift_test) {
  const Region<2> memory { { 666, 1999 }, { 8, 9 } };
  const auto mapping = makeMemRegion(memory);
  BOOST_TEST(mapping.fileToMemory() == memory.front);
  BOOST_TEST(mapping.memoryToFile() == -memory.front);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
