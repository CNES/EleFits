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

#include "EleFitsData/TestRaster.h"
#include "EleFits/FitsFileFixture.h"
#include "EleFits/MefFile.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>
#include <cstdio>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(MefFile_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(primary_index_is_consistent_test, Test::TemporaryMefFile) {
  const auto& primary = this->primary();
  BOOST_TEST(primary.index() == MefFile::primaryIndex);
}

BOOST_FIXTURE_TEST_CASE(primary_resize_test, Test::NewMefFile) {
  Test::SmallRaster input; // TODO RandomRaster
  const auto& primary = this->primary();
  primary.updateShape<float, 2>(input.shape());
  primary.writeRaster(input);
  this->close();
  // Reopen as read-only
  this->open(this->filename(), FileMode::Read);
  const auto output = this->primary().readRaster<float, 2>();
  remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(count_test, Test::TemporaryMefFile) {
  BOOST_TEST(this->hduCount() == 1); // 0 with CFitsIO
  Test::SmallRaster raster;
  const auto& primary = this->primary();
  primary.updateShape<float, 2>(raster.shape());
  BOOST_TEST(this->hduCount() == 1);
  const auto& ext = this->initImageExt<float, 2>("IMG", raster.shape());
  BOOST_TEST(this->hduCount() == 2); // 1 with CFitsIO
  ext.writeRaster(raster);
  BOOST_TEST(this->hduCount() == 2);
}

BOOST_FIXTURE_TEST_CASE(append_test, Test::NewMefFile) {
  Test::SmallRaster raster; // TODO RandomRaster
  const auto& ext1 = this->assignImageExt("IMG1", raster);
  BOOST_TEST(ext1.index() == 1);
  BOOST_TEST(this->hduCount() == 2);
  this->close();
  // Reopen as read-only
  this->open(this->filename(), FileMode::Edit);
  BOOST_TEST(this->hduCount() == 2);
  const auto& ext2 = this->assignImageExt("IMG2", raster);
  BOOST_TEST(ext2.index() == 2);
  BOOST_TEST(this->hduCount() == 3);
  std::vector<std::string> inputNames { "", "IMG1", "IMG2" };
  const auto outputNames = this->readHduNames();
  BOOST_TEST(outputNames == inputNames);
  remove(this->filename().c_str());
}

BOOST_FIXTURE_TEST_CASE(reaccess_hdu_and_use_previous_reference_test, Test::TemporaryMefFile) {
  const auto& firstlyAccessedPrimary = this->primary();
  BOOST_CHECK_NO_THROW(firstlyAccessedPrimary.readName());
  this->initImageExt<float, 2>("IMG", {});
  const auto& secondlyAccessedPrimary = this->primary();
  BOOST_TEST(firstlyAccessedPrimary.readName() == secondlyAccessedPrimary.readName());
}

BOOST_FIXTURE_TEST_CASE(access_single_named_hdu, Test::TemporaryMefFile) {
  const std::string extname = "EXT";
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
  this->initRecordExt(extname);
  BOOST_CHECK_NO_THROW(this->access<>(extname));
  this->initRecordExt(extname);
  BOOST_CHECK_THROW(this->access<>(extname), FitsError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
