/**
 * @file tests/src/TypeWrapper_test.cpp
 * @date 07/23/19
 * @author user
 *
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

#include "EL_CfitsioWrapper//TypeWrapper.h"

using namespace Euclid;
using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (TypeWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_cfitsio_record_type(int code) {
    BOOST_CHECK_EQUAL(TypeCode<T>::for_record(), code);
}

template<typename T>
void check_cfitsio_bintable_type(int code) {
    BOOST_CHECK_EQUAL(TypeCode<T>::for_bintable(), code);
}

template<typename T>
void check_cfitsio_image_type(int code) {
    BOOST_CHECK_EQUAL(TypeCode<T>::for_image(), code);
}

BOOST_AUTO_TEST_CASE( cfitsio_record_codes_test ) {

    check_cfitsio_record_type<bool>(TLOGICAL);
    check_cfitsio_record_type<char>(TSBYTE);
    check_cfitsio_record_type<short>(TSHORT);
    check_cfitsio_record_type<int>(TINT);
    check_cfitsio_record_type<long>(TLONG);
    check_cfitsio_record_type<long long>(TLONGLONG);
    check_cfitsio_record_type<float>(TFLOAT);
    check_cfitsio_record_type<double>(TDOUBLE);
    check_cfitsio_record_type<std::complex<float>>(TCOMPLEX);
    check_cfitsio_record_type<std::complex<double>>(TDBLCOMPLEX);
    check_cfitsio_record_type<std::string>(TSTRING);
    check_cfitsio_record_type<unsigned char>(TBYTE);
    check_cfitsio_record_type<unsigned short>(TUSHORT);
    check_cfitsio_record_type<unsigned int>(TUINT);
    check_cfitsio_record_type<unsigned long>(TULONG);

}

BOOST_AUTO_TEST_CASE( cfitsio_bintable_codes_test ) {

    check_cfitsio_bintable_type<bool>(TBIT);
    check_cfitsio_bintable_type<char>(TSBYTE);
    check_cfitsio_bintable_type<short>(TSHORT);
    check_cfitsio_bintable_type<int>(TINT);
    check_cfitsio_bintable_type<long>(TLONG);
    check_cfitsio_bintable_type<float>(TFLOAT);
    check_cfitsio_bintable_type<double>(TDOUBLE);
    check_cfitsio_bintable_type<std::complex<float>>(TCOMPLEX);
    check_cfitsio_bintable_type<std::complex<double>>(TDBLCOMPLEX);
    check_cfitsio_bintable_type<std::string>(TSTRING);
    check_cfitsio_bintable_type<unsigned char>(TBYTE);
    check_cfitsio_bintable_type<unsigned short>(TUSHORT);
    check_cfitsio_bintable_type<unsigned int>(TUINT);
    check_cfitsio_bintable_type<unsigned long>(TULONG);

}

BOOST_AUTO_TEST_CASE( cfitsio_image_codes_test ) {

    check_cfitsio_image_type<char>(BYTE_IMG);
    check_cfitsio_image_type<short>(SHORT_IMG);
    check_cfitsio_image_type<long>(LONG_IMG);
    check_cfitsio_image_type<float>(FLOAT_IMG);
    check_cfitsio_image_type<double>(DOUBLE_IMG);
    check_cfitsio_image_type<unsigned char>(BYTE_IMG);
    check_cfitsio_image_type<unsigned short>(USHORT_IMG);
    // check_cfitsio_image_type<unsigned long>(ULONG_IMG);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


