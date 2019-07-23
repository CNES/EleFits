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
void check_cfitsio_type(int expected_code) {
    BOOST_CHECK_EQUAL(cfitsio_type<T>::code, expected_code);
}

BOOST_AUTO_TEST_CASE( cfitsio_type_code_test ) {

    check_cfitsio_type<bool>(TLOGICAL); //TODO check mapping
    check_cfitsio_type<unsigned char>(TBYTE);
    check_cfitsio_type<bool>(TLOGICAL);
    check_cfitsio_type<std::string>(TSTRING);
    check_cfitsio_type<short>(TSHORT);
    check_cfitsio_type<long>(TLONG);
    check_cfitsio_type<long long>(TLONGLONG);
    check_cfitsio_type<float>(TFLOAT);
    check_cfitsio_type<double>(TDOUBLE);
    check_cfitsio_type<int>(TINT);
    check_cfitsio_type<char>(TSBYTE);
    check_cfitsio_type<unsigned int>(TUINT);
    check_cfitsio_type<unsigned short>(TUSHORT);
    check_cfitsio_type<unsigned long>(TULONG);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


