// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FileMemSegments.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FileMemSegments_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(append_resolve_test) {
  long fileBack = 9;
  long memoryFront = 20;
  long memoryBack = 99;
  long size = memoryBack - memoryFront + 1;

  FileMemSegments segments {-1, {memoryFront, -1}};
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

  FileMemSegments segments {fileFront, {memoryFront, -1}};
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

  FileMemSegments segments {{fileFront, -1}, memoryFront};
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
