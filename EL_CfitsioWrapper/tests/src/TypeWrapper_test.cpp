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

BOOST_AUTO_TEST_CASE(cfitsio_record_codes_test) {
  checkCfitsioRecordTypeExists<bool>();
  checkCfitsioRecordTypeExists<char>();
  checkCfitsioRecordTypeExists<short>();
  checkCfitsioRecordTypeExists<int>();
  checkCfitsioRecordTypeExists<long>();
  checkCfitsioRecordTypeExists<float>();
  checkCfitsioRecordTypeExists<double>();
  checkCfitsioRecordTypeExists<std::complex<float>>();
  checkCfitsioRecordTypeExists<std::complex<double>>();
  checkCfitsioRecordTypeExists<std::string>();
  checkCfitsioRecordTypeExists<unsigned char>();
  checkCfitsioRecordTypeExists<unsigned short>();
  checkCfitsioRecordTypeExists<unsigned int>();
  checkCfitsioRecordTypeExists<unsigned long>();
}

BOOST_AUTO_TEST_CASE(cfitsio_bintable_codes_test) {
  checkCfitsioBintableTypeExists<bool>();
  checkCfitsioBintableTypeExists<char>();
  checkCfitsioBintableTypeExists<short>();
  checkCfitsioBintableTypeExists<int>();
  checkCfitsioBintableTypeExists<long>();
  checkCfitsioBintableTypeExists<float>();
  checkCfitsioBintableTypeExists<double>();
  checkCfitsioBintableTypeExists<std::complex<float>>();
  checkCfitsioBintableTypeExists<std::complex<double>>();
  checkCfitsioBintableTypeExists<std::string>();
  checkCfitsioBintableTypeExists<unsigned char>();
  checkCfitsioBintableTypeExists<unsigned short>();
  checkCfitsioBintableTypeExists<unsigned int>();
  checkCfitsioBintableTypeExists<unsigned long>();
}

BOOST_AUTO_TEST_CASE(cfitsio_image_codes_test) {
  checkCfitsioImageTypeExists<char>();
  checkCfitsioImageTypeExists<short>();
  checkCfitsioImageTypeExists<int>();
  checkCfitsioImageTypeExists<long>();
  checkCfitsioImageTypeExists<float>();
  checkCfitsioImageTypeExists<double>();
  checkCfitsioImageTypeExists<unsigned char>();
  checkCfitsioImageTypeExists<unsigned short>();
  checkCfitsioImageTypeExists<unsigned int>();
  checkCfitsioImageTypeExists<unsigned long>();
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
