// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/FitsFile.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(empty_file_test) {

  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  BOOST_TEST(not boost::filesystem::exists(filename));

  FitsFile newFile(filename, FileMode::Create);
  BOOST_TEST(newFile.filename() == filename);
  BOOST_TEST(boost::filesystem::is_regular_file(filename));
  newFile.close();

  BOOST_CHECK_THROW(FitsFile(filename, FileMode::Create), std::exception);

  FitsFile overwrittenFile(filename, FileMode::Overwrite);
  overwrittenFile.close();

  FitsFile readonlyFile(filename, FileMode::Read);
  BOOST_CHECK_THROW(readonlyFile.closeAndDelete(), std::exception);
  readonlyFile.close();

  FitsFile editableFile(filename, FileMode::Edit);
  editableFile.closeAndDelete();
  BOOST_TEST(not boost::filesystem::exists(filename));

  {
    FitsFile tempFile(filename, FileMode::Temporary);
    BOOST_TEST(boost::filesystem::is_regular_file(filename));
  }
  BOOST_TEST(not boost::filesystem::exists(filename));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
