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
