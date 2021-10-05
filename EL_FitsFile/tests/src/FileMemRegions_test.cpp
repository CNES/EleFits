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

#include "EL_FitsFile/FileMemRegions.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FileMemRegions_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctor_test) {
  const Position<2> file { 6, 8 };
  const Position<2> memory { 3, 4 };
  const Position<2> shape { 7, 9 };

  const FileMemRegions<2> region_position(Region<2>::fromShape(file, shape), memory);
  BOOST_TEST(region_position.file().shape() == shape);
  BOOST_TEST(region_position.file().front == file);
  BOOST_TEST(region_position.memory().shape() == shape);
  BOOST_TEST(region_position.memory().front == memory);

  const FileMemRegions<2> position_region(file, Region<2>::fromShape(memory, shape));
  BOOST_TEST(position_region.file().shape() == shape);
  BOOST_TEST(position_region.file().front == file);
  BOOST_TEST(position_region.memory().shape() == shape);
  BOOST_TEST(position_region.memory().front == memory);

  const FileMemRegions<2> region(Region<2>::fromShape(file, shape));
  BOOST_TEST(region.file().shape() == shape);
  BOOST_TEST(region.file().front == file);
  BOOST_TEST(region.memory().shape() == shape);
  BOOST_TEST(region.memory().front == file);
}

BOOST_AUTO_TEST_CASE(shift_test) {
  const Region<2> memory { { 666, 1999 }, { 8, 9 } };
  const auto mapping = makeMemRegion(memory);
  BOOST_TEST(mapping.fileToMemory() == memory.front);
  BOOST_TEST(mapping.memoryToFile() == -memory.front);
}

BOOST_AUTO_TEST_CASE(max_test) {
  const auto region = Region<2>::whole();
  FileMemRegions<2> mapping(region);
  BOOST_TEST((mapping.file() == region));
  BOOST_TEST(mapping.memory().front == Position<2>::zero());
  BOOST_TEST(mapping.memory().back == Position<2>::zero());
  const Position<2> newBack { 1, 2 };
  mapping.setFileBackIfMax(newBack);
  BOOST_TEST(mapping.file().front == Position<2>::zero());
  BOOST_TEST(mapping.file().back == newBack);
  BOOST_TEST(mapping.memory().front == Position<2>::zero());
  BOOST_TEST(mapping.memory().back != Position<2>::zero());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
