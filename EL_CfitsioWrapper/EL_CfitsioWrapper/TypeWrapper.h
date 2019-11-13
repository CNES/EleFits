/**
 * @file EL_CfitsioWrapper/TypeWrapper.h
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
#include <complex>
#include <string>
#include <vector>

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {
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
     * @brief Get the TFORM value to handle Bintable columns.
     */
    inline static std::string bintable_format(int width) {
        may_throw_cfitsio_error(BAD_DATATYPE);
        return "";
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

/**
 * @brief Pointer specialization of TypeCode.
 */
template<typename T>
struct TypeCode<T*> {

	inline static int for_record() {
		return TypeCode<T>::for_record();
	}

	inline static int for_bintable() {
		return TypeCode<T>::for_bintable();
	}

	inline static std::string bintable_format(int width) {
		return TypeCode<T>::bintable_format(width);
	}

	inline static int for_image() {
		return TypeCode<T>::for_image();
	}

	inline static int bitpix() {
		return TypeCode<T>::bitpix();
	}

};

/**
 * @brief Vector specialization of TypeCode for Bintable vector columns.
 */
template<typename T>
struct TypeCode<std::vector<T>> {

	inline static int for_bintable() {
		return TypeCode<T*>::for_bintable();
	}

	inline static std::string bintable_format(int width) {
		return TypeCode<T*>::bintable_format(width);
	}

};


/////////////////////
// IMPLEMENTATION //
///////////////////


/// @cond IMPLEMENTATION

#define DEF_RECORD_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_record() { return code; }

#define DEF_TABLE_TYPE_CODE(type, code, column) \
    template<> inline int TypeCode<type>::for_bintable() { return code; } \
    template<> inline std::string TypeCode<type>::bintable_format(int width) { return std::to_string(width) + column; }

#define DEF_IMAGE_TYPE_CODE(type, code) \
    template<> inline int TypeCode<type>::for_image() { return code; }

#define DEF_IMAGE_BITPIX(type, code) \
    template<> inline int TypeCode<type>::bitpix() { return code; }


/*
 * From CFitsIO documentation "Keyword Reading Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node38.html
 * 
 * Allowed types for records:
 * TSTRING, TLOGICAL (== int), TBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TULONG, TLONGLONG, TFLOAT, TDOUBLE, TCOMPLEX, and TDBLCOMPLEX
 */
DEF_RECORD_TYPE_CODE(bool, TLOGICAL)
DEF_RECORD_TYPE_CODE(char, TSBYTE)
DEF_RECORD_TYPE_CODE(short, TSHORT) // not std::int16_t for "some" reason...
DEF_RECORD_TYPE_CODE(int, TINT)
DEF_RECORD_TYPE_CODE(long, TLONG) // not std::int32_t
DEF_RECORD_TYPE_CODE(LONGLONG, TLONGLONG) // not std::int64_t
DEF_RECORD_TYPE_CODE(float, TFLOAT)
DEF_RECORD_TYPE_CODE(double, TDOUBLE)
DEF_RECORD_TYPE_CODE(std::complex<float>, TCOMPLEX)
DEF_RECORD_TYPE_CODE(std::complex<double>, TDBLCOMPLEX)
DEF_RECORD_TYPE_CODE(std::string, TSTRING)
DEF_RECORD_TYPE_CODE(char*, TSTRING)
DEF_RECORD_TYPE_CODE(unsigned char, TBYTE)
DEF_RECORD_TYPE_CODE(unsigned short, TUSHORT) // not std::uint16_t
DEF_RECORD_TYPE_CODE(unsigned int, TUINT)
DEF_RECORD_TYPE_CODE(unsigned long, TULONG) // not std::uint32_t
//DEF_RECORD_TYPE_CODE(std::uint64_t, TULONGLONG) // Not defined in EDEN 2.0 version

/*
 * From CFitsIO documentation "Read and Write Column Data Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node46.html
 *
 * Allowed types for ASCII tables:
 * TSTRING, TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 * Additionnal types for binary tables:
 * TLOGICAL (internally mapped to the `char' data type), TCOMPLEX, TDBLCOMPLEX
 */
DEF_TABLE_TYPE_CODE(bool, TBIT, 'X')
DEF_TABLE_TYPE_CODE(char, TSBYTE, 'A')
DEF_TABLE_TYPE_CODE(short, TSHORT, 'I')
DEF_TABLE_TYPE_CODE(int, TINT, 'J')
DEF_TABLE_TYPE_CODE(long, TLONG, 'K')
DEF_TABLE_TYPE_CODE(float, TFLOAT, 'E')
DEF_TABLE_TYPE_CODE(double, TDOUBLE, 'D')
DEF_TABLE_TYPE_CODE(std::complex<float>, TCOMPLEX, 'C')
DEF_TABLE_TYPE_CODE(std::complex<double>, TDBLCOMPLEX, 'M')
DEF_TABLE_TYPE_CODE(std::string, TSTRING, 'A')
DEF_TABLE_TYPE_CODE(char*, TSTRING, 'A')
DEF_TABLE_TYPE_CODE(unsigned char, TBYTE, 'B')
DEF_TABLE_TYPE_CODE(unsigned short, TUSHORT, 'I')
DEF_TABLE_TYPE_CODE(unsigned int, TUINT, 'J')
DEF_TABLE_TYPE_CODE(unsigned long, TULONG, 'K')
//DEF_TABLE_TYPE_CODE(std::uint64_t, TULONGLONG, 'K') // Not defined in our version

/*
 * From CFitsIO documentation "Primary Array or IMAGE Extension I/O Routines"
 * https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node40.html
 *
 * Allowed types:
 * TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TULONGLONG, TFLOAT, TDOUBLE
 */
DEF_IMAGE_TYPE_CODE(char, TSBYTE)
DEF_IMAGE_TYPE_CODE(std::int16_t, TSHORT)
DEF_IMAGE_TYPE_CODE(std::int32_t, TLONG)
DEF_IMAGE_TYPE_CODE(std::int64_t, TLONGLONG)
DEF_IMAGE_TYPE_CODE(float, TFLOAT)
DEF_IMAGE_TYPE_CODE(double, TDOUBLE)
DEF_IMAGE_TYPE_CODE(unsigned char, TBYTE)
DEF_IMAGE_TYPE_CODE(std::uint16_t, TUSHORT)
DEF_IMAGE_TYPE_CODE(std::uint32_t, TULONG)
//DEF_IMAGE_TYPE_CODE(std::uint64_t, TULONGLONG) // Not defined in our version

/*
 * From CFitsIO documentation?
 *
 * Allowed types:
 * BYTE_IMG, SHORT_IMG, LONG_IMG, LONGLONG_IMG, FLOAT_IMG, DOUBLE_IMG
 * //TODO Check:
 * SBYTE_IMG, USHORT_IMG, ULONG_IMG, ULONGLONG_IMG
 */
DEF_IMAGE_BITPIX(char, SBYTE_IMG)
DEF_IMAGE_BITPIX(std::int16_t, SHORT_IMG)
DEF_IMAGE_BITPIX(std::int32_t, LONG_IMG)
DEF_IMAGE_BITPIX(std::int64_t, LONGLONG_IMG)
DEF_IMAGE_BITPIX(float, FLOAT_IMG)
DEF_IMAGE_BITPIX(double, DOUBLE_IMG)
DEF_IMAGE_BITPIX(unsigned char, BYTE_IMG)
DEF_IMAGE_BITPIX(std::uint16_t, USHORT_IMG)
DEF_IMAGE_BITPIX(std::uint32_t, ULONG_IMG)
//DEF_IMAGE_BITPIX(std::uint64_t, ULONGLONG_IMG) // Not defined in our version

/// @endcond

}
}

#endif
