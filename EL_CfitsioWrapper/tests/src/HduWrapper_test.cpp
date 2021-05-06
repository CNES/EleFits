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

#include "EL_FitsData/TestColumn.h"
#include "EL_FitsData/TestRaster.h"

#include "EL_CfitsioWrapper/CfitsioFixture.h"
#include "EL_CfitsioWrapper/HduWrapper.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduWrapper_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(minimal_file_has_accessible_primary_test, FitsIO::Test::MinimalFile) {
  Hdu::gotoPrimary(this->fptr);
  BOOST_CHECK_EQUAL(Hdu::currentIndex(this->fptr), 1);
  BOOST_CHECK(Hdu::currentIsPrimary(this->fptr));
}

BOOST_FIXTURE_TEST_CASE(create_and_access_image_extension_test, FitsIO::Test::MinimalFile) {
  using namespace FitsIO::Test;
  SmallRaster input;
  Hdu::createImageExtension(this->fptr, "IMGEXT", input);
  BOOST_CHECK_EQUAL(Hdu::currentIndex(this->fptr), 2);
  BOOST_CHECK(Hdu::currentType(this->fptr) == FitsIO::HduType::Image);
  Hdu::gotoNext(this->fptr, -1);
  BOOST_CHECK_EQUAL(Hdu::currentIndex(this->fptr), 1);
  Hdu::gotoName(this->fptr, "IMGEXT");
  BOOST_CHECK_EQUAL(Hdu::currentIndex(this->fptr), 2);
  const auto output = Image::readRaster<float, 2>(fptr);
  checkEqualVectors(output.vector(), input.vector());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
