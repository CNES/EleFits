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

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleFitsData/TestColumn.h"
#include "EleFitsData/TestRaster.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(HduWrapper_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(minimal_file_has_accessible_primary_test, Fits::Test::MinimalFile) {
  HduAccess::gotoPrimary(this->fptr);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == 1);
  BOOST_TEST(HduAccess::currentIsPrimary(this->fptr));
}

BOOST_FIXTURE_TEST_CASE(create_and_access_image_extension_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallRaster input;
  HduAccess::assignImageExtension(this->fptr, "IMGEXT", input);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == 2);
  BOOST_TEST((HduAccess::currentType(this->fptr) == Fits::HduCategory::Image));
  HduAccess::gotoNext(this->fptr, -1);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == 1);
  HduAccess::gotoName(this->fptr, "IMGEXT");
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == 2);
  const auto output = ImageIo::readRaster<float, 2>(fptr);
  BOOST_TEST(output.vector() == input.vector());
}

BOOST_FIXTURE_TEST_CASE(access_hdu_by_type, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  const std::string name = "NAME";
  constexpr long primaryIndex = 1;
  HduAccess::assignImageExtension(this->fptr, name, SmallRaster());
  constexpr long imageIndex = primaryIndex + 1;
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == imageIndex);
  HduAccess::assignBintableExtension(this->fptr, name, SmallTable().nameCol);
  constexpr long bintableIndex = imageIndex + 1;
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == bintableIndex);
  HduAccess::gotoName(this->fptr, name);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == imageIndex);
  HduAccess::gotoName(this->fptr, name, 0, Fits::HduCategory::Image);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == imageIndex);
  HduAccess::gotoName(this->fptr, name, 0, Fits::HduCategory::Bintable);
  BOOST_TEST(HduAccess::currentIndex(this->fptr) == bintableIndex);

  // TODO test extver
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
