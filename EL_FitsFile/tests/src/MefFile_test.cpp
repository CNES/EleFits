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

#include "EL_FitsData/FitsDataFixture.h"
#include "EL_FitsFile/MefFile.h"

using namespace Euclid::FitsIO;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MefFile_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( primary_resize_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::Overwrite);
  Test::SmallRaster input;
  const auto& primary = f.accessPrimary<ImageHdu>();
  primary.resize<float, 2>(input.shape);
  primary.writeRaster(input);
  f.close();
  f.open(filename, MefFile::Permission::Read);
  const auto output = f.accessPrimary<ImageHdu>().readRaster<float, 2>();
}

BOOST_AUTO_TEST_CASE( count_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::Temporary);
  BOOST_CHECK_EQUAL(f.hduCount(), 1); // 0 with CFitsIO
  Test::SmallRaster raster;
  const auto& primary = f.accessPrimary<ImageHdu>();
  primary.resize<float, 2>(raster.shape);
  BOOST_CHECK_EQUAL(f.hduCount(), 1);
  const auto& ext = f.initImageExt<float, 2>("IMG", raster.shape);
  BOOST_CHECK_EQUAL(f.hduCount(), 2); // 1 with CFitsIO
  ext.writeRaster(raster);
  BOOST_CHECK_EQUAL(f.hduCount(), 2);
}

BOOST_AUTO_TEST_CASE( append_test ) {
  Elements::TempPath tmp("%%%%%%.fits");
  std::string filename = tmp.path().string();
  MefFile f(filename, MefFile::Permission::Overwrite);
  Test::SmallRaster raster;
  const auto& ext1 = f.assignImageExt("IMG1", raster);
  BOOST_CHECK_EQUAL(ext1.index(), 2);
  BOOST_CHECK_EQUAL(f.hduCount(), 2);
  f.close();
  f.open(filename, MefFile::Permission::Edit);
  BOOST_CHECK_EQUAL(f.hduCount(), 2);
  const auto& ext2 = f.assignImageExt("IMG2", raster);
  BOOST_CHECK_EQUAL(ext2.index(), 3);
  BOOST_CHECK_EQUAL(f.hduCount(), 3);
  std::vector<std::string> inputNames { "", "IMG1", "IMG2" };
  const auto outputNames = f.hduNames();
  BOOST_CHECK_EQUAL_COLLECTIONS(outputNames.begin(), outputNames.end(), inputNames.begin(), inputNames.end());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


