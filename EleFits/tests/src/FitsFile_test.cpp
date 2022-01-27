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
