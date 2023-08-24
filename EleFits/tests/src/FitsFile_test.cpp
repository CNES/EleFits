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

  FitsFile new_file(filename, FileMode::Create);
  BOOST_TEST(new_file.filename() == filename);
  BOOST_TEST(boost::filesystem::is_regular_file(filename));
  new_file.close();

  BOOST_CHECK_THROW(FitsFile(filename, FileMode::Create), std::exception);

  FitsFile overwritten_file(filename, FileMode::Overwrite);
  overwritten_file.close();

  FitsFile readonly_file(filename);
  BOOST_CHECK_THROW(readonly_file.close_remove(), std::exception);
  readonly_file.close();

  FitsFile editable_file(filename, FileMode::Edit);
  editable_file.close_remove();
  BOOST_TEST(not boost::filesystem::exists(filename));

  {
    FitsFile temp_file(filename, FileMode::Temporary);
    BOOST_TEST(boost::filesystem::is_regular_file(filename));
  }
  BOOST_TEST(not boost::filesystem::exists(filename));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
