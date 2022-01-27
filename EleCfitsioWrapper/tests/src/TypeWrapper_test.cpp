/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

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
