// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/TestColumn.h" // Macro to loop on types
#include "EleFitsData/TestRaster.h" // Macro to loop on types
#include "EleFitsData/TestRecord.h" // Macro to loop on types

#include <boost/test/unit_test.hpp>

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(TypeWrapper_test)

//-----------------------------------------------------------------------------

template <typename T>
void checkCfitsioRecordTypeExists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::forRecord());
}

template <typename T>
void checkCfitsioBintableTypeExists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::forBintable());
  BOOST_CHECK_NO_THROW(TypeCode<T>::tform(1));
}

template <typename T>
void checkCfitsioImageTypeExists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::forImage());
  BOOST_CHECK_NO_THROW(TypeCode<T>::bitpix());
}

#define RECORD_TYPECODE_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_typecode_test) { \
    checkCfitsioRecordTypeExists<type>(); \
  }

#define BINTABLE_TFORM_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_bintable_tform_test) { \
    checkCfitsioBintableTypeExists<type>(); \
  }

#define IMAGE_BITPIX_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_image_bitpix_test) { \
    checkCfitsioImageTypeExists<type>(); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_TYPECODE_TEST)
ELEFITS_FOREACH_COLUMN_TYPE(BINTABLE_TFORM_TEST)
ELEFITS_FOREACH_RASTER_TYPE(IMAGE_BITPIX_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
