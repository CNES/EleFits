// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/FileWrapper.h"

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FileWrapper_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(file_operations_test, Fits::Test::MinimalFile) {
  BOOST_CHECK_THROW(FileAccess::createAndOpen(filename, FileAccess::CreatePolicy::CreateOnly), CfitsioError);
  BOOST_TEST(fptr);
  FileAccess::close(fptr);
  BOOST_TEST(boost::filesystem::is_regular_file(filename));
  BOOST_TEST(not fptr);
  fptr = FileAccess::open(filename, FileAccess::OpenPolicy::ReadOnly);
  BOOST_TEST(fptr);
  BOOST_CHECK_THROW(FileAccess::closeAndDelete(fptr), CfitsioError);
  FileAccess::close(fptr);
  fptr = FileAccess::open(filename, FileAccess::OpenPolicy::ReadWrite);
  FileAccess::closeAndDelete(fptr);
  BOOST_TEST(not boost::filesystem::exists(filename));
  BOOST_TEST(not fptr);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
