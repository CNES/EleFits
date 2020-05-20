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

BOOST_AUTO_TEST_SUITE (TypeWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_cfitsio_record_type_exists() {
    BOOST_CHECK_NO_THROW(TypeCode<T>::for_record());
}

template<typename T>
void check_cfitsio_bintable_type_exists() {
    BOOST_CHECK_NO_THROW(TypeCode<T>::for_bintable());
}

template<typename T>
void check_cfitsio_image_type_exists() {
    BOOST_CHECK_NO_THROW(TypeCode<T>::for_image());
}

BOOST_AUTO_TEST_CASE( cfitsio_record_codes_test ) {

    check_cfitsio_record_type_exists<bool>();
    check_cfitsio_record_type_exists<char>();
    check_cfitsio_record_type_exists<short>();
    check_cfitsio_record_type_exists<int>();
    check_cfitsio_record_type_exists<long>();
    check_cfitsio_record_type_exists<float>();
    check_cfitsio_record_type_exists<double>();
    check_cfitsio_record_type_exists<std::complex<float>>();
    check_cfitsio_record_type_exists<std::complex<double>>();
    check_cfitsio_record_type_exists<std::string>();
    check_cfitsio_record_type_exists<unsigned char>();
    check_cfitsio_record_type_exists<unsigned short>();
    check_cfitsio_record_type_exists<unsigned int>();
    check_cfitsio_record_type_exists<unsigned long>();

}

BOOST_AUTO_TEST_CASE( cfitsio_bintable_codes_test ) {

    check_cfitsio_bintable_type_exists<bool>();
    check_cfitsio_bintable_type_exists<char>();
    check_cfitsio_bintable_type_exists<short>();
    check_cfitsio_bintable_type_exists<int>();
    check_cfitsio_bintable_type_exists<long>();
    check_cfitsio_bintable_type_exists<float>();
    check_cfitsio_bintable_type_exists<double>();
    check_cfitsio_bintable_type_exists<std::complex<float>>();
    check_cfitsio_bintable_type_exists<std::complex<double>>();
    check_cfitsio_bintable_type_exists<std::string>();
    check_cfitsio_bintable_type_exists<unsigned char>();
    check_cfitsio_bintable_type_exists<unsigned short>();
    check_cfitsio_bintable_type_exists<unsigned int>();
    check_cfitsio_bintable_type_exists<unsigned long>();

}

BOOST_AUTO_TEST_CASE( cfitsio_image_codes_test ) {

    check_cfitsio_image_type_exists<char>();
    check_cfitsio_image_type_exists<short>();
    check_cfitsio_image_type_exists<int>();
    check_cfitsio_image_type_exists<long>();
    check_cfitsio_image_type_exists<float>();
    check_cfitsio_image_type_exists<double>();
    check_cfitsio_image_type_exists<unsigned char>();
    check_cfitsio_image_type_exists<unsigned short>();
    check_cfitsio_image_type_exists<unsigned int>();
    check_cfitsio_image_type_exists<unsigned long>();

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
