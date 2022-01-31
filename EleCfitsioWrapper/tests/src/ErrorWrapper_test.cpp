// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/ErrorWrapper.h"

#include <boost/test/unit_test.hpp>

using namespace Euclid::Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(ErrorWrapper_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(noerror_test) {
  BOOST_CHECK_NO_THROW(CfitsioError::mayThrow(0));
}

BOOST_AUTO_TEST_CASE(error_test) {
  BOOST_CHECK_THROW(CfitsioError::mayThrow(1), CfitsioError);
}

BOOST_AUTO_TEST_CASE(nullptr_test) {
  BOOST_CHECK_THROW(mayThrowInvalidFileError(nullptr), CfitsioError);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
