/**
 * @file tests/src/SifFile_test.cpp
 * @date 08/30/19
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

#include "ElementsKernel/Temporary.h"

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_FitsFile/SifFile.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SifFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( simple_image_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  filename = "/tmp/test.fits";
  SifFile f(filename, SifFile::Permission::OVERWRITE);//, SifFile::Permission::TEMPORARY);
  BOOST_CHECK(boost::filesystem::is_regular_file(filename));
  Test::SmallRaster input;
  f.write_raster<float>(input);
  const int value = 8;
  f.header().write_record("KEYWORD", value);
  BOOST_CHECK_EQUAL(f.header().parse_record<int>("KEYWORD"), value);
  f.close();
  f.open(filename, SifFile::Permission::READ);
  const auto output = f.read_raster<float>();
  BOOST_CHECK_EQUAL_COLLECTIONS(input.vector().begin(), input.vector().end(), output.vector().begin(), output.vector().end());
  BOOST_CHECK(input.approx(output));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
