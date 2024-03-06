// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FileMemRegions.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FileMemRegions_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctor_test)
{
  const Linx::Position<2> file {6, 8};
  const Linx::Position<2> memory {3, 4};
  const Linx::Position<2> shape {7, 9};

  const FileMemRegions<2> region_position(Linx::Box<2>::from_shape(file, shape), memory);
  BOOST_TEST(region_position.file().shape() == shape);
  BOOST_TEST(region_position.file().front() == file);
  BOOST_TEST(region_position.memory().shape() == shape);
  BOOST_TEST(region_position.memory().front() == memory);

  const FileMemRegions<2> position_region(file, Linx::Box<2>::from_shape(memory, shape));
  BOOST_TEST(position_region.file().shape() == shape);
  BOOST_TEST(position_region.file().front() == file);
  BOOST_TEST(position_region.memory().shape() == shape);
  BOOST_TEST(position_region.memory().front() == memory);

  const FileMemRegions<2> region(Linx::Box<2>::from_shape(file, shape));
  BOOST_TEST(region.file().shape() == shape);
  BOOST_TEST(region.file().front() == file);
  BOOST_TEST(region.memory().shape() == shape);
  BOOST_TEST(region.memory().front() == Linx::Position<2>::zero());
}

BOOST_AUTO_TEST_CASE(shift_test)
{
  const Linx::Box<2> memory {{666, 1999}, {8, 9}};
  const auto mapping = makeMemRegion(memory);
  BOOST_TEST(mapping.fileToMemory() == memory.front());
  BOOST_TEST(mapping.memoryToFile() == -memory.front());
}

BOOST_AUTO_TEST_CASE(max_test)
{
  const auto region = FileMemRegions<3>::whole();
  FileMemRegions<3> mapping(region);
  BOOST_TEST((mapping.file() == region));
  BOOST_TEST(mapping.memory().front() == Linx::Position<3>::zero());
  BOOST_TEST(mapping.memory().back() == Linx::Position<3>::zero());
  const Linx::Position<3> fileBack {2, 8, 5};
  mapping.resolve(fileBack, Linx::Position<3>::zero());
  BOOST_TEST(mapping.file().front() == Linx::Position<3>::zero());
  BOOST_TEST(mapping.file().back() == fileBack);
  BOOST_TEST(mapping.memory().front() == Linx::Position<3>::zero());
  BOOST_TEST(mapping.memory().back() != Linx::Position<3>::zero());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
