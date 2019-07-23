/**
 * @file src/lib/TypeHandler.cpp
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

#include "EL_CFitsIOWrapper/TypeWrapper.h"

namespace Cfitsio {

//TODO TBIT

template<> const int cfitsio_type<unsigned char>::code = TBYTE;
template<> const int cfitsio_type<bool>::code = TLOGICAL;
template<> const int cfitsio_type<std::string>::code = TSTRING;
template<> const int cfitsio_type<short>::code = TSHORT;
template<> const int cfitsio_type<long>::code = TLONG;
template<> const int cfitsio_type<long long>::code = TLONGLONG;
template<> const int cfitsio_type<float>::code = TFLOAT;
template<> const int cfitsio_type<double>::code = TDOUBLE;
//TODO TCOMPLEX
//TODO TDBLCOMPLEX
template<> const int cfitsio_type<int>::code = TINT;
template<> const int cfitsio_type<char>::code = TSBYTE;
template<> const int cfitsio_type<unsigned int>::code = TUINT;
template<> const int cfitsio_type<unsigned short>::code = TUSHORT;
template<> const int cfitsio_type<unsigned long>::code = TULONG;

}
