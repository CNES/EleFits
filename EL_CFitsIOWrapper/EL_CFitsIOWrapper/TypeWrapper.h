/**
 * @file EL_CFitsIOWrapper/TypeWrapper.h
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

#ifndef _EL_CFITSIOWRAPPER_TYPEWRAPPER_H
#define _EL_CFITSIOWRAPPER_TYPEWRAPPER_H

#include <cfitsio/fitsio.h>
#include <stdexcept>
#include <typeinfo>

#include <complex>
#include <limits>
#include <string>

#include "EL_CFitsIOWrapper/ErrorWrapper.h"

namespace Cfitsio {

/**
 * @brief Type traits to convert C++ types to CFitsIO type codes.
 * 
 * Used to read and write:
 * * Record's,
 * * Image's,
 * * Bintable's (ASCII table not supported).
 */
template<typename T>
struct TypeCode;

template<typename T>
struct TypeCode {

    /**
     * @brief Get the type code for a Record.
     */
    inline static int for_record() {
        may_throw_cfitsio_error(BAD_DATATYPE);
        return 0;
    }

    /**
     * @brief Get the type code for a Bintable.
     */
    inline static int for_bintable() {
        may_throw_cfitsio_error(BAD_DATATYPE);
        return 0;
    }

    /**
     * @brief Get the type code for an Image.
     */
    inline static int for_image() {
        may_throw_cfitsio_error(BAD_DATATYPE);
        return 0;
    }

    /**
     * @brief Get the BITPIX value to handle Image HDUs.
     */
    inline static int bitpix() {
        may_throw_cfitsio_error(BAD_DATATYPE);
        return 0;
    }

};

#define DEF_RECORD_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_record() { return code; }

#define DEF_TABLE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_bintable() { return code; }

#define DEF_IMAGE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_image() { return code; }

#define DEF_IMAGE_BITPIX(type, code) \
    template<> inline int TypeCode<type>::bitpix() { return code; }

DEF_RECORD_TYPE_CODE(bool, TLOGICAL)
DEF_RECORD_TYPE_CODE(char, TSBYTE)
DEF_RECORD_TYPE_CODE(short, TSHORT)
DEF_RECORD_TYPE_CODE(int, TINT)
DEF_RECORD_TYPE_CODE(long, TLONG)
/*
 * From CFitsIO documentation "64-Bit Long Integers"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node117.html
 * LONGLONG adapts to the system encoding for 64-bit integers (= long or long long)
 */
DEF_RECORD_TYPE_CODE(LONGLONG, TLONGLONG)
DEF_RECORD_TYPE_CODE(float, TFLOAT)
DEF_RECORD_TYPE_CODE(double, TDOUBLE)
DEF_RECORD_TYPE_CODE(std::complex<float>, TCOMPLEX)
DEF_RECORD_TYPE_CODE(std::complex<double>, TDBLCOMPLEX)
DEF_RECORD_TYPE_CODE(std::string, TSTRING)
DEF_RECORD_TYPE_CODE(char*, TSTRING)
DEF_RECORD_TYPE_CODE(unsigned char, TBYTE)
DEF_RECORD_TYPE_CODE(unsigned short, TUSHORT)
DEF_RECORD_TYPE_CODE(unsigned int, TUINT)
DEF_RECORD_TYPE_CODE(unsigned long, TULONG)
//DEF_RECORD_TYPE_CODE(unsigned LONGLONG, TULONGLONG) // Not defined in our version

/*
 * From CFitsIO documentation "Read and Write Column Data Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node46.html
 *
 * Allowed types for ASCII tables:
 * TSTRING, TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 * Additionnal types for binary tables:
 * TLOGICAL (internally mapped to the `char' data type), TCOMPLEX, TDBLCOMPLEX
 */
DEF_TABLE_TYPE_CODE(bool, TBIT)
DEF_TABLE_TYPE_CODE(char, TSBYTE) //TODO should it be TLOGICAL???
DEF_TABLE_TYPE_CODE(short, TSHORT)
DEF_TABLE_TYPE_CODE(int, TINT)
DEF_TABLE_TYPE_CODE(long, TLONG) //TODO TLONGLONG in TFits => check
DEF_TABLE_TYPE_CODE(LONGLONG, TLONGLONG)
DEF_TABLE_TYPE_CODE(float, TFLOAT)
DEF_TABLE_TYPE_CODE(double, TDOUBLE)
DEF_TABLE_TYPE_CODE(std::complex<float>, TCOMPLEX)
DEF_TABLE_TYPE_CODE(std::complex<double>, TDBLCOMPLEX)
DEF_TABLE_TYPE_CODE(std::string, TSTRING)
DEF_TABLE_TYPE_CODE(char*, TSTRING)
DEF_TABLE_TYPE_CODE(unsigned char, TBYTE)
DEF_TABLE_TYPE_CODE(unsigned short, TUSHORT)
DEF_TABLE_TYPE_CODE(unsigned int, TUINT)
DEF_TABLE_TYPE_CODE(unsigned long, TULONG)
//DEF_TABLE_TYPE_CODE(unsigned LONGLONG, TULONGLONG) // Not defined in our version

/*
 * From CFitsIO documentation "Primary Array or IMAGE Extension I/O Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node40.html
 *
 * Allowed types:
 * TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 */
DEF_IMAGE_TYPE_CODE(char, TSBYTE)
DEF_IMAGE_TYPE_CODE(short, TSHORT)
DEF_IMAGE_TYPE_CODE(int, TINT)
DEF_IMAGE_TYPE_CODE(long, TLONG)
DEF_IMAGE_TYPE_CODE(LONGLONG, TLONGLONG)
DEF_IMAGE_TYPE_CODE(float, TFLOAT)
DEF_IMAGE_TYPE_CODE(double, TDOUBLE)
DEF_IMAGE_TYPE_CODE(unsigned char, TBYTE)
DEF_IMAGE_TYPE_CODE(unsigned short, TUSHORT)
DEF_IMAGE_TYPE_CODE(unsigned int, TUINT)
DEF_IMAGE_TYPE_CODE(unsigned long, TULONG)
//DEF_IMAGE_TYPE_CODE(unsigned LONGLONG, TULONGLONG) // Not defined in our version

/*
 * From CFitsIO documentation?
 *
 * Allowed types:
 * BYTE_IMG, SHORT_IMG, LONG_IMG, LONGLONG_IMG, FLOAT_IMG, DOUBLE_IMG
 * //TODO Check:
 * SBYTE_IMG, USHORT_IMG, ULONG_IMG, ULONGLONG_IMG
 */
DEF_IMAGE_BITPIX(char, SBYTE_IMG)
DEF_IMAGE_BITPIX(short, SHORT_IMG)
DEF_IMAGE_BITPIX(long, LONG_IMG)
DEF_IMAGE_BITPIX(LONGLONG, LONGLONG_IMG)
DEF_IMAGE_BITPIX(float, FLOAT_IMG)
DEF_IMAGE_BITPIX(double, DOUBLE_IMG)
DEF_IMAGE_BITPIX(unsigned char, BYTE_IMG)
DEF_IMAGE_BITPIX(unsigned short, USHORT_IMG)
DEF_IMAGE_BITPIX(unsigned long, ULONG_IMG)
//DEF_IMAGE_BITPIX(unsigned LONGLONG, ULONGLONG_IMG)

}

#endif

