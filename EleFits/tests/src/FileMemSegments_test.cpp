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

#include "EleFits/FileMemSegments.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FileMemSegments_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(append_resolve_test) {
  long fileBack = 9;
  long memoryFront = 20;
  long memoryBack = 99;
  long size = memoryBack - memoryFront + 1;

  FileMemSegments segments { -1, { memoryFront, -1 } };
  BOOST_TEST(segments.file().front == -1);
  BOOST_TEST(segments.file().back == -2);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == -1);

  segments.resolve(fileBack, memoryBack);
  BOOST_TEST(segments.file().front == fileBack + 1);
  BOOST_TEST(segments.file().back == fileBack + size);
  BOOST_TEST(segments.file().size() == size);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == memoryBack);
  BOOST_TEST(segments.memory().size() == size);
}

BOOST_AUTO_TEST_CASE(write_resolve_test) {
  long fileFront = 10;
  long fileBack = -2;
  long memoryFront = 19;
  long memoryBack = 99;
  long size = memoryBack - memoryFront + 1;

  FileMemSegments segments { fileFront, { memoryFront, -1 } };
  BOOST_TEST(segments.file().front == fileFront);
  BOOST_TEST(segments.file().back == -2);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == -1);

  segments.resolve(fileBack, memoryBack);
  BOOST_TEST(segments.file().front == fileFront);
  BOOST_TEST(segments.file().back == fileFront + size - 1);
  BOOST_TEST(segments.file().size() == size);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == memoryBack);
  BOOST_TEST(segments.memory().size() == size);
}

BOOST_AUTO_TEST_CASE(partial_read_resolve_test) {
  long fileFront = 200;
  long fileBack = 999;
  long memoryFront = 10;
  long memoryBack = -2;
  long size = fileBack - fileFront + 1;

  FileMemSegments segments { { fileFront, -1 }, memoryFront };
  BOOST_TEST(segments.file().front == fileFront);
  BOOST_TEST(segments.file().back == -1);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == -2);

  segments.resolve(fileBack, memoryBack);
  BOOST_TEST(segments.file().front == fileFront);
  BOOST_TEST(segments.file().back == fileBack);
  BOOST_TEST(segments.file().size() == size);
  BOOST_TEST(segments.memory().front == memoryFront);
  BOOST_TEST(segments.memory().back == memoryFront + size - 1);
  BOOST_TEST(segments.memory().size() == size);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
