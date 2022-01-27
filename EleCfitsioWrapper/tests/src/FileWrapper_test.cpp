/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
