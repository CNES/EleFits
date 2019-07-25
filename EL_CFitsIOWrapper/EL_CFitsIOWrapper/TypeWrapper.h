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

namespace Cfitsio {

template<typename T>
struct TypeCode;

template<typename T>
class TypeError : public std::runtime_error {
public:
    TypeError() :
        std::runtime_error("Unknown type: " + TypeCode<T>::type_name()) {}
};

/**
 * Type traits to convert C++ types to CFitsIO type codes for:
 * * records,
 * * images,
 * * bintables (ASCII tables are not supported).
 */
template<typename T>
struct TypeCode {

    /**
     * Get the type code for a record.
     */
    inline static int for_record() {
        throw TypeError<T>();
    }

    /**
     * Get the type code for a bintable.
     */
    inline static int for_bintable() {
        throw TypeError<T>();
    }

    /**
     * Get the type code for an image.
     */
    inline static int for_image() {
        throw TypeError<T>();
    }

    /**
     * Get the compiler type name.
     */
    inline static std::string type_name() {
        return typeid(T).name();
    }

};

#define DEF_RECORD_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_record() { return code; }
#define DEF_TABLE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_bintable() { return code; }
#define DEF_IMAGE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_image() { return code; }

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

}

#endif

