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

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"

#include "EL_FitsFile/FitsFile.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FitsFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( empty_file_test ) {

  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  BOOST_CHECK(not boost::filesystem::exists(filename));
  
  FitsFile new_file(filename, FitsFile::Permission::CREATE);
  BOOST_CHECK_EQUAL(new_file.filename(), filename);
  BOOST_CHECK(boost::filesystem::is_regular_file(filename));
  new_file.close();
  
  BOOST_CHECK_THROW(FitsFile(filename, FitsFile::Permission::CREATE), std::exception);
  
  FitsFile overwritten_file(filename, FitsFile::Permission::OVERWRITE);
  overwritten_file.close();
  
  FitsFile read_only_file(filename, FitsFile::Permission::READ);
  BOOST_CHECK_THROW(read_only_file.close_and_delete(), std::exception);
  read_only_file.close();
  
  FitsFile editable_file(filename, FitsFile::Permission::EDIT);
  editable_file.close_and_delete();
  BOOST_CHECK(not boost::filesystem::exists(filename));
  
  {
    FitsFile temp_file(filename, FitsFile::Permission::TEMPORARY);
    BOOST_CHECK(boost::filesystem::is_regular_file(filename));
  }
  BOOST_CHECK(not boost::filesystem::exists(filename));
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
