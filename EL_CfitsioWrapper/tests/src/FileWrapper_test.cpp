/**
 * @file tests/src/FileWrapper_test.cpp
 * @date 07/23/19
 * @author user
 *
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

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "EL_CfitsioWrapper//ErrorWrapper.h"
#include "EL_CfitsioWrapper//FileWrapper.h"
#include "EL_CfitsioWrapper//CfitsioFixture.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FileWrapper_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE( file_operations_test, FitsIO::Test::MinimalFile ) {
    BOOST_CHECK_THROW(File::create_and_open(filename, File::CreatePolicy::CREATE_ONLY), CfitsioError);
    BOOST_CHECK(fptr);
    File::close(fptr);
    BOOST_CHECK(boost::filesystem::is_regular_file(filename));
    BOOST_CHECK(not fptr);
    fptr = File::open(filename, File::OpenPolicy::READ_ONLY);
    BOOST_CHECK(fptr);
    BOOST_CHECK_THROW(File::close_and_delete(fptr), CfitsioError);
    File::close(fptr);
    fptr = File::open(filename, File::OpenPolicy::READ_WRITE);
    File::close_and_delete(fptr);
    BOOST_CHECK(not boost::filesystem::exists(filename));
    BOOST_CHECK(not fptr);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


