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

#include "EL_CfitsioWrapper/TypeWrapper.h"
#include "EL_FitsData/TestColumn.h" // EL_FITSIO_LOOP...
#include "EL_FitsData/TestRaster.h" // EL_FITSIO_LOOP...
#include "EL_FitsData/TestRecord.h" // EL_FITSIO_LOOP...

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
}

template <typename T>
void checkCfitsioImageTypeExists() {
  BOOST_CHECK_NO_THROW(TypeCode<T>::forImage());
}

#define TEST_RECORD_TYPECODE(type, name) \
  BOOST_AUTO_TEST_CASE(name##_typecode_test) { \
    checkCfitsioRecordTypeExists<type>(); \
  }

#define TEST_BINTABLE_TFORM(type, name) \
  BOOST_AUTO_TEST_CASE(name##_tform_test) { \
    checkCfitsioBintableTypeExists<type>(); \
  }

#define TEST_IMAGE_BITPIX(type, name) \
  BOOST_AUTO_TEST_CASE(name##_bitpix_test) { \
    checkCfitsioImageTypeExists<type>(); \
  }

EL_FITSIO_FOREACH_RECORD_TYPE(TEST_RECORD_TYPECODE)
EL_FITSIO_FOREACH_COLUMN_TYPE(TEST_BINTABLE_TFORM)
EL_FITSIO_FOREACH_RASTER_TYPE(TEST_IMAGE_BITPIX)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
