// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/FileWrapper.h"

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

using namespace Euclid::Cfitsio;
using namespace Euclid::Fits::Test;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioFixture_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(temporary_file_is_removed_by_destructor_test) {
  std::string filename;
  {
    MinimalFile f;
    filename = f.filename;
    BOOST_TEST(boost::filesystem::is_regular_file(filename));
  }
  BOOST_TEST(not boost::filesystem::is_regular_file(filename));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
