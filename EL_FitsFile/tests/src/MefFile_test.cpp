/**
 * @file tests/src/MefFile_test.cpp
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
#include "EL_FitsFile/MefFile.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MefFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( primary_resize_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::OVERWRITE);
  Test::SmallRaster input;
  const auto& primary = f.access_primary<ImageHdu>();
  primary.resize<float, 2>(input.shape);
  primary.write_raster(input);
  f.close();
  f.open(filename, MefFile::Permission::READ);
  const auto output = f.access_primary<ImageHdu>().read_raster<float, 2>();
}

BOOST_AUTO_TEST_CASE( count_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::TEMPORARY);
  BOOST_CHECK_EQUAL(f.complete_hdu_count(), 0);
  Test::SmallRaster raster;
  const auto& primary = f.access_primary<ImageHdu>();
  primary.resize<float, 2>(raster.shape);
  BOOST_CHECK_EQUAL(f.complete_hdu_count(), 1);
  const auto& ext = f.init_image_ext<float, 2>("IMG", raster.shape);
  BOOST_CHECK_EQUAL(f.complete_hdu_count(), 1);
  ext.write_raster(raster);
  BOOST_CHECK_EQUAL(f.complete_hdu_count(), 2);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


