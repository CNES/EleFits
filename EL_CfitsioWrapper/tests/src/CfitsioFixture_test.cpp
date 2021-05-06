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

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/FileWrapper.h"

using namespace Euclid::Cfitsio;
using namespace Euclid::FitsIO::Test;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(CfitsioFixture_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(temporary_file_is_removed_by_destructor_test) {
  std::string filename;
  {
    MinimalFile f;
    filename = f.filename;
    BOOST_CHECK(boost::filesystem::is_regular_file(filename));
  }
  BOOST_CHECK(not boost::filesystem::is_regular_file(filename));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
