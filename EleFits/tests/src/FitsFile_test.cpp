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

#include "EleFits/FitsFile.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(FitsFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(empty_file_test) {

  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  BOOST_TEST(not boost::filesystem::exists(filename));

  FitsFile newFile(filename, FitsFile::Permission::Create);
  BOOST_TEST(newFile.filename() == filename);
  BOOST_TEST(boost::filesystem::is_regular_file(filename));
  newFile.close();

  BOOST_CHECK_THROW(FitsFile(filename, FitsFile::Permission::Create), std::exception);

  FitsFile overwrittenFile(filename, FitsFile::Permission::Overwrite);
  overwrittenFile.close();

  FitsFile readonlyFile(filename, FitsFile::Permission::Read);
  BOOST_CHECK_THROW(readonlyFile.closeAndDelete(), std::exception);
  readonlyFile.close();

  FitsFile editableFile(filename, FitsFile::Permission::Edit);
  editableFile.closeAndDelete();
  BOOST_TEST(not boost::filesystem::exists(filename));

  {
    FitsFile tempFile(filename, FitsFile::Permission::Temporary);
    BOOST_TEST(boost::filesystem::is_regular_file(filename));
  }
  BOOST_TEST(not boost::filesystem::exists(filename));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
