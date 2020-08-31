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

#ifndef _EL_CFITSIOWRAPPER_TYPEWRAPPER_H
#define _EL_CFITSIOWRAPPER_TYPEWRAPPER_H

#include <fitsio.h>
#include <complex>
#include <string>
#include <vector>

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Type traits to convert C++ types to CFitsIO type codes.
 * @details
 * Used to read and write:
 * * Record's,
 * * Image's,
 * * Bintable's (ASCII table not supported).
 */
template<typename T>
struct TypeCode {

    /**
     * @brief Get the type code for a Record.
     */
    inline static int forRecord();

    /**
     * @brief Get the type code for a Bintable.
     */
    inline static int forBintable();

    /**
     * @brief Get the TFORM value to handle Bintable columns.
     */
    inline static std::string tform(long repeat);

    /**
     * @brief Get the type code for an Image.
     */
    inline static int forImage();

    /**
     * @brief Get the BITPIX value to handle Image HDUs.
     */
    inline static int bitpix();

};


/////////////////////
// IMPLEMENTATION //
///////////////////


/// @cond IMPLEMENTATION

/*
 * From CFitsIO documentation "Keyword Reading Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node38.html
 *
 * Allowed types for records:
 * TSTRING, TLOGICAL (== int), TBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TULONG, TLONGLONG,
 * TFLOAT, TDOUBLE, TCOMPLEX, and TDBLCOMPLEX
 */
#ifndef DEF_RECORD_TYPE_CODE
#define DEF_RECORD_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::forRecord() { return code; }
DEF_RECORD_TYPE_CODE(bool, TLOGICAL)
DEF_RECORD_TYPE_CODE(char, TSBYTE)
DEF_RECORD_TYPE_CODE(short, TSHORT)
DEF_RECORD_TYPE_CODE(int, TINT)
DEF_RECORD_TYPE_CODE(long, TLONG)
DEF_RECORD_TYPE_CODE(long long, TLONGLONG)
DEF_RECORD_TYPE_CODE(float, TFLOAT)
DEF_RECORD_TYPE_CODE(double, TDOUBLE)
DEF_RECORD_TYPE_CODE(std::complex<float>, TCOMPLEX)
DEF_RECORD_TYPE_CODE(std::complex<double>, TDBLCOMPLEX)
DEF_RECORD_TYPE_CODE(std::string, TSTRING)
DEF_RECORD_TYPE_CODE(unsigned char, TBYTE)
DEF_RECORD_TYPE_CODE(unsigned short, TUSHORT)
DEF_RECORD_TYPE_CODE(unsigned int, TUINT)
DEF_RECORD_TYPE_CODE(unsigned long, TULONG)
DEF_RECORD_TYPE_CODE(unsigned long long, TULONGLONG)
#undef DEF_RECORD_TYPE_CODE
#endif

/*
 * From CFitsIO documentation "Read and Write Column Data Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node46.html
 *
 * Allowed types for ASCII tables:
 * TSTRING, TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 * Additionnal types for binary tables:
 * TLOGICAL (internally mapped to the `char' data type), TCOMPLEX, TDBLCOMPLEX
 */
#ifndef DEF_TABLE_TYPE_CODE
#define DEF_TABLE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::forBintable() { return code; }
DEF_TABLE_TYPE_CODE(bool, TBIT)
DEF_TABLE_TYPE_CODE(char, TSBYTE)
DEF_TABLE_TYPE_CODE(short, TSHORT)
DEF_TABLE_TYPE_CODE(int, TINT)
DEF_TABLE_TYPE_CODE(long, TLONG)
DEF_TABLE_TYPE_CODE(float, TFLOAT)
DEF_TABLE_TYPE_CODE(double, TDOUBLE)
DEF_TABLE_TYPE_CODE(std::complex<float>, TCOMPLEX)
DEF_TABLE_TYPE_CODE(std::complex<double>, TDBLCOMPLEX)
DEF_TABLE_TYPE_CODE(std::string, TSTRING)
DEF_TABLE_TYPE_CODE(unsigned char, TBYTE)
DEF_TABLE_TYPE_CODE(unsigned short, TUSHORT)
DEF_TABLE_TYPE_CODE(unsigned int, TUINT)
DEF_TABLE_TYPE_CODE(unsigned long, TULONG)
DEF_TABLE_TYPE_CODE(unsigned long long, TULONGLONG)
#undef DEF_TABLE_TYPE_CODE
#endif

/*
 * From FITS standart and CFitsIO documentation "Read and Write Column Data Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node23.html
 *
 * CFITSIO recognizes 4 additional data type codes besides those already defined in the FITS standard:
 * - `U' meaning a 16-bit unsigned integer column,
 * - `V' for a 32-bit unsigned integer column,
 * - `W' for a 64-bit unsigned integer column, and
 * - 'S' for a signed byte column.
 */
#ifndef DEF_TABLE_TFORM
#define DEF_TABLE_TFORM(type, code) \
    template<> inline std::string TypeCode<type>::tform(long repeat) { return std::to_string(repeat) + code; }
DEF_TABLE_TFORM(bool, 'X')
DEF_TABLE_TFORM(char, 'S')
DEF_TABLE_TFORM(std::int16_t, 'I')
DEF_TABLE_TFORM(std::int32_t, 'J')
DEF_TABLE_TFORM(std::int64_t, 'K')
DEF_TABLE_TFORM(float, 'E')
DEF_TABLE_TFORM(double, 'D')
DEF_TABLE_TFORM(std::complex<float>, 'C')
DEF_TABLE_TFORM(std::complex<double>, 'M')
DEF_TABLE_TFORM(std::string, 'A')
DEF_TABLE_TFORM(unsigned char, 'B')
DEF_TABLE_TFORM(std::uint16_t, 'U')
DEF_TABLE_TFORM(std::uint32_t, 'V')
DEF_TABLE_TFORM(std::uint64_t, 'W')
#undef DEF_TABLE_TFORM
#endif

/*
 * From CFitsIO documentation "Primary Array or Image Extension I/O Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node40.html
 *
 * Allowed types:
 * TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 */
#ifndef DEF_IMAGE_TYPE_CODE
#define DEF_IMAGE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::forImage() { return code; }
DEF_IMAGE_TYPE_CODE(char, TSBYTE)
DEF_IMAGE_TYPE_CODE(short, TSHORT)
DEF_IMAGE_TYPE_CODE(int, TINT)
DEF_IMAGE_TYPE_CODE(long, TLONG)
DEF_IMAGE_TYPE_CODE(long long, TLONGLONG)
DEF_IMAGE_TYPE_CODE(float, TFLOAT)
DEF_IMAGE_TYPE_CODE(double, TDOUBLE)
DEF_IMAGE_TYPE_CODE(unsigned char, TBYTE)
DEF_IMAGE_TYPE_CODE(unsigned short, TUSHORT)
DEF_IMAGE_TYPE_CODE(unsigned int, TUINT)
DEF_IMAGE_TYPE_CODE(unsigned long, TULONG)
DEF_IMAGE_TYPE_CODE(unsigned long long, TULONGLONG)
#undef DEF_IMAGE_TYPE_CODE
#endif

/*
 * From CFitsIO documentation? //TODO link
 *
 * Allowed types:
 * BYTE_IMG, SHORT_IMG, LONG_IMG, LONGLONG_IMG, FLOAT_IMG, DOUBLE_IMG
 * SBYTE_IMG, USHORT_IMG, ULONG_IMG, ULONGLONG_IMG
 */
#ifndef DEF_IMAGE_BITPIX
#define DEF_IMAGE_BITPIX(type, code) \
    template<> inline int TypeCode<type>::bitpix() { return code; }
DEF_IMAGE_BITPIX(char, SBYTE_IMG)
DEF_IMAGE_BITPIX(std::int16_t, SHORT_IMG)
DEF_IMAGE_BITPIX(std::int32_t, LONG_IMG)
DEF_IMAGE_BITPIX(std::int64_t, LONGLONG_IMG)
DEF_IMAGE_BITPIX(float, FLOAT_IMG)
DEF_IMAGE_BITPIX(double, DOUBLE_IMG)
DEF_IMAGE_BITPIX(unsigned char, BYTE_IMG)
DEF_IMAGE_BITPIX(std::uint16_t, USHORT_IMG)
DEF_IMAGE_BITPIX(std::uint32_t, ULONG_IMG)
DEF_IMAGE_BITPIX(std::uint64_t, ULONGLONG_IMG)
#undef DEF_IMAGE_BITPIX
#endif

/// @endcond

}
}

#endif
