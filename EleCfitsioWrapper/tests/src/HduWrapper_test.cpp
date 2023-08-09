// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"
#include "EleCfitsioWrapper/CompressionWrapper.h"
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
  HduAccess::goto_primary(this->fptr);
  BOOST_TEST(HduAccess::current_index(this->fptr) == 1);
  BOOST_TEST(HduAccess::current_is_primary(this->fptr));
}

BOOST_FIXTURE_TEST_CASE(create_and_access_image_extension_test, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  SmallRaster input;
  HduAccess::assign_image(this->fptr, "IMGEXT", input);
  BOOST_TEST(HduAccess::current_index(this->fptr) == 2);
  BOOST_TEST((HduAccess::current_type(this->fptr) == Fits::HduCategory::Image));
  HduAccess::goto_next(this->fptr, -1);
  BOOST_TEST(HduAccess::current_index(this->fptr) == 1);
  HduAccess::goto_name(this->fptr, "IMGEXT");
  BOOST_TEST(HduAccess::current_index(this->fptr) == 2);
  const auto output = ImageIo::read_raster<float, 2>(fptr);
  BOOST_TEST(output.vector() == input.vector());
}

BOOST_FIXTURE_TEST_CASE(access_hdu_by_type, Fits::Test::MinimalFile) {
  using namespace Fits::Test;
  const std::string name = "NAME";
  constexpr long primaryIndex = 1;
  HduAccess::assign_image(this->fptr, name, SmallRaster());
  constexpr long imageIndex = primaryIndex + 1;
  BOOST_TEST(HduAccess::current_index(this->fptr) == imageIndex);
  HduAccess::assign_bintable(this->fptr, name, SmallTable().name_col);
  constexpr long bintableIndex = imageIndex + 1;
  BOOST_TEST(HduAccess::current_index(this->fptr) == bintableIndex);
  HduAccess::goto_name(this->fptr, name);
  BOOST_TEST(HduAccess::current_index(this->fptr) == imageIndex);
  HduAccess::goto_name(this->fptr, name, 0, Fits::HduCategory::Image);
  BOOST_TEST(HduAccess::current_index(this->fptr) == imageIndex);
  HduAccess::goto_name(this->fptr, name, 0, Fits::HduCategory::Bintable);
  BOOST_TEST(HduAccess::current_index(this->fptr) == bintableIndex);

  // TODO test extver
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
