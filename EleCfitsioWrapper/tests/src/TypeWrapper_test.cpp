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
void check_cfitsio_record_type_exists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::for_record());
}

template <typename T>
void check_cfitsio_bintable_type_exists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::for_bintable());
  BOOST_CHECK_NO_THROW(TypeCode<T>::tform(1));
}

template <typename T>
void check_cfitsio_image_type_exists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::for_image());
  BOOST_CHECK_NO_THROW(TypeCode<T>::bitpix());
}

#define RECORD_TYPECODE_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_record_typecode_test) { \
    check_cfitsio_record_type_exists<type>(); \
  }

#define BINTABLE_TFORM_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_bintable_tform_test) { \
    check_cfitsio_bintable_type_exists<type>(); \
  }

#define IMAGE_BITPIX_TEST(type, name) \
  BOOST_AUTO_TEST_CASE(name##_image_bitpix_test) { \
    check_cfitsio_image_type_exists<type>(); \
  }

ELEFITS_FOREACH_RECORD_TYPE(RECORD_TYPECODE_TEST)
ELEFITS_FOREACH_COLUMN_TYPE(BINTABLE_TFORM_TEST)
ELEFITS_FOREACH_RASTER_TYPE(IMAGE_BITPIX_TEST)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
