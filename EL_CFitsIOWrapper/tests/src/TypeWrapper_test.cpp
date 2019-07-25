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

#include "EL_CFitsIOWrapper//TypeWrapper.h"

using namespace Cfitsio;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (TypeWrapper_test)

//-----------------------------------------------------------------------------

template<typename T>
void check_cfitsio_types(int record, int table, int image) {
    if(record)
        BOOST_CHECK_EQUAL(TypeCode<T>::for_record(), record);
    else
        BOOST_CHECK_THROW(TypeCode<T>::for_record(), TypeError<T>);
    if(table)
        BOOST_CHECK_EQUAL(TypeCode<T>::for_bintable(), table);
    else
        BOOST_CHECK_THROW(TypeCode<T>::for_bintable(), TypeError<T>);
    if(image)
        BOOST_CHECK_EQUAL(TypeCode<T>::for_image(), image);
    else
        BOOST_CHECK_THROW(TypeCode<T>::for_image(), TypeError<T>);
}

BOOST_AUTO_TEST_CASE( cfitsio_type_codes_test ) {

    check_cfitsio_types<bool>(TLOGICAL, TBIT, 0);
    check_cfitsio_types<char>(TSBYTE, TSBYTE, TSBYTE);
    check_cfitsio_types<short>(TSHORT, TSHORT, TSHORT);
    check_cfitsio_types<int>(TINT, TINT, TINT);
    check_cfitsio_types<long>(TLONG, TLONG, TLONG);
    check_cfitsio_types<LONGLONG>(TLONGLONG, TLONGLONG, TLONGLONG);
    check_cfitsio_types<float>(TFLOAT, TFLOAT, TFLOAT);
    check_cfitsio_types<double>(TDOUBLE, TDOUBLE, TDOUBLE);
    check_cfitsio_types<std::complex<float>>(TCOMPLEX, TCOMPLEX, 0);
    check_cfitsio_types<std::complex<double>>(TDBLCOMPLEX, TDBLCOMPLEX, 0);
    check_cfitsio_types<std::string>(TSTRING, TSTRING, 0);
    check_cfitsio_types<char*>(TSTRING, TSTRING, 0);
    check_cfitsio_types<unsigned char>(TBYTE, TBYTE, TBYTE);
    check_cfitsio_types<unsigned short>(TUSHORT, TUSHORT, TUSHORT);
    check_cfitsio_types<unsigned int>(TUINT, TUINT, TUINT);
    check_cfitsio_types<unsigned long>(TULONG, TULONG, TULONG);
    check_cfitsio_types<unsigned long long>(0, 0, 0);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


