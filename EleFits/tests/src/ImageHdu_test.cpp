// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ImageHdu.h"
#include "EleFits/MefFile.h"
#include "EleFitsData/TestRaster.h"
#include "ElementsKernel/Temporary.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Fits;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ImageHdu_test)

//-----------------------------------------------------------------------------

template <typename T>
void check_2d() {
  Test::RandomRaster<T, 2> input({16, 9});
  const std::string filename = Elements::TempFile().path().string();
  MefFile file(filename, FileMode::Temporary);
  file.assignImageExt("IMGEXT", input);
  const auto output = file.accessFirst<ImageHdu>("IMGEXT").readRaster<T, 2>();
  BOOST_TEST(output.vector() == input.vector());
}

/**
 * We test only one type here to check the flow from the top-level API to CFITSIO.
 * Support for other types is tested in EleCfitsioWrapper.
 */
BOOST_AUTO_TEST_CASE(float_test) {
  check_2d<float>();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
