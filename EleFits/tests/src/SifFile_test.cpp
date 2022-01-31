// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFileFixture.h"
#include "EleFits/SifFile.h"
#include "EleFitsData/TestRaster.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(SifFile_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(simple_image_test, Test::NewSifFile) {
  BOOST_TEST(boost::filesystem::is_regular_file(this->filename()));
  Test::SmallRaster input; // TODO RandomRaster
  const std::string keyword = "KEYWORD";
  const int value = 8;
  this->header().write(keyword, value);
  this->writeRaster(input);
  this->close();
  this->open(this->filename(), FileMode::Read); // Reopen as read-only
  const auto record = this->header().parse<int>(keyword);
  BOOST_TEST((record == value));
  const auto output = this->readRaster<float>();
  BOOST_TEST(input.vector() == output.vector());
  BOOST_TEST(input.approx(output));
  remove(this->filename().c_str());
}

BOOST_AUTO_TEST_CASE(write_all_test) {
  Test::SmallRaster input;
  const Record<int> i {"INT", 1, "i", "integer"};
  const Record<std::string> s {"STR", "TWO", "s", "text"};
  SifFile f(Elements::TempPath().path().string(), FileMode::Temporary);
  f.writeAll({i, s}, input);
  const auto records = f.header().parseAll();
  const auto output = f.raster().read<float>();
  auto r = records.as<int>(i.keyword);
  BOOST_TEST((records.as<int>(i.keyword) == i));
  BOOST_TEST((records.as<std::string>(s.keyword) == s));
  BOOST_TEST(output.vector() == input.vector());
}

BOOST_FIXTURE_TEST_CASE(checksum_test, Test::TemporarySifFile) {
  BOOST_CHECK_THROW(this->verifyChecksums(), ChecksumError);
  this->updateChecksums();
  BOOST_CHECK_NO_THROW(this->verifyChecksums());
  this->header().write("DATASUM", std::string(""));
  BOOST_CHECK_THROW(this->verifyChecksums(), ChecksumError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
