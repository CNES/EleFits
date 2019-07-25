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

/**

Type code for       record      table       image
--------------------------------------------------------
bool                TLOGICAL    TBIT        -
char                TSBYTE      TSBYTE?     SBYTE_IMG
short?              TSHORT      TSHORT      SHORT_IMG
int	                TINT?       -           -
long?               TLONG       TLONGLONG   LONG_IMG
long long?          TLONGLONG   TLONGLONG   LONGLONG_IMG
float               TFLOAT      TFLOAT      FLOAT_IMG
double              TDOUBLE     TDOUBLE     DOUBLE_IMG
complex<float>      TCOMPLEX    TCOMPLEX    -
complex<double>     TDBLCOMPLEX TDBLCOMPLEX -
string              TSTRING     TSTRING     -
unsigned char       TBYTE       TBYTE       BYTE_IMG
unsigned short?     TUSHORT     TUSHORT     USHORT_IMG
unsigned int        TUINT       TUINT       -
unsigned long?      TULONG      TULONGLONG? ULONG_IMG
unsigned long long?	TULONGLONG? TULONGLONG? ULONGLONG_IMG

/!\ Wrong image table
/!\ See LONGLONG typedef

 */
template<typename T>
struct TypeCode;

template<typename T>
class TypeError : public std::runtime_error {
public:
    TypeError() :
        std::runtime_error("Unknown type: " + TypeCode<T>::type_name()) {}
};

template<typename T>
struct TypeCode {

    inline static int for_record() {
        throw TypeError<T>();
    }

    inline static int for_table() {
        throw TypeError<T>();
    }

    inline static int for_image() {
        throw TypeError<T>();
    }

    inline static std::string type_name() {
        return typeid(T).name();
    }

};


/*
template<typename T>
struct TypeCode<T*> : public TypeCode<T> {};
*/

#define DEF_RECORD_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_record() { return code; }
#define DEF_TABLE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_table() { return code; }
#define DEF_IMAGE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_image() { return code; }

DEF_RECORD_TYPE_CODE(bool, TLOGICAL)
DEF_RECORD_TYPE_CODE(char, TSBYTE)
DEF_RECORD_TYPE_CODE(short, TSHORT)
DEF_RECORD_TYPE_CODE(int, TINT)
DEF_RECORD_TYPE_CODE(long, TLONG)
DEF_RECORD_TYPE_CODE(LONGLONG, TLONGLONG)
// LONGLONG adapts to the system encoding for 64-bit integers (= long or long long)
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
//DEF_RECORD_TYPE_CODE(unsigned long long, TULONGLONG) // Not defined in our version

DEF_TABLE_TYPE_CODE(bool, TBIT)
DEF_TABLE_TYPE_CODE(char, TSBYTE)
DEF_TABLE_TYPE_CODE(short, TSHORT)
DEF_TABLE_TYPE_CODE(long, TLONGLONG)
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
//DEF_TABLE_TYPE_CODE(unsigned long, TULONGLONG) // Not defined in our version
//DEF_TABLE_TYPE_CODE(unsigned long, TULONGLONG) // Not defined in our version

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
//DEF_IMAGE_TYPE_CODE(unsigned long long, TULONGLONG) // Not defined in our version

}

#endif

