/**
 * @file EL_CfitsioWrapper/RecordHandler.h
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

#ifndef _EL_CFITSIOWRAPPER_RECORDHANDLER_H
#define _EL_CFITSIOWRAPPER_RECORDHANDLER_H

#include <cfitsio/fitsio.h>
#include <string>
#include <tuple>
#include <vector>

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Cfitsio {

/**
 * @brief Record-related functions.
 * 
 * A record is a keyword-value pair with optional unit and comment.
 */
namespace Record {

/**
 * @brief Type for record, i.e. { keyword, value, unit, comment }
 */
template<typename T>
using record_type = std::tuple<std::string, T, std::string, std::string>;

/**
 * @brief Read the value of a given keyword with specified type.
 */
template<typename T>
T parse_value(fitsfile* fptr, std::string keyword);

/**
 * @brief Read the values of a given set of keywords with specified types.
 */
template<typename... Ts>
std::tuple<Ts...> parse_values(fitsfile* fptr, std::vector<std::string> keywords);

/**
 * @brief Read the value of a given keyword as a string.
 */
std::string read_value(fitsfile* fptr, std::string keyword);

/**
 * @brief Read the values of a given set of keywords as a set of strings.
 */
std::vector<std::string> read_values(fitsfile* fptr, std::vector<std::string> keywords);

/**
 * @brief Write a new record with given keyword and value.
 */
template<typename T>
void write_value(fitsfile* fptr, std::string keyword, T value);

/**
 * @brief Write a new record with unit and comment.
 */
template<typename T>
void write_record(fitsfile* fptr, const record_type<T>& record);

/**
 * @brief Update an existing record or write a new one with given keyword and value.
 */
template<typename T>
void update_value(fitsfile* fptr, std::string keyword, T value);


///////////////
// INTENRAL //
/////////////


/// @cond INTERNAL
namespace internal {

// Signature change (output argument) for further use with variadic templates.
template<typename T>
inline void _parse_value(fitsfile* fptr, std::string keyword, T& value) {
    value = parse_value<T>(fptr, keyword);
}

// Parse the values of the i+1 first keywords of a given list (recursive approach).
template<int i, typename ...Ts>
struct _parse_values {
    void operator() (fitsfile* fptr, std::vector<std::string> keywords, std::tuple<Ts...>& values) {
        _parse_value(fptr, keywords[i], std::get<i>(values));
        _parse_values<i-1, Ts...>{}(fptr, keywords, values);
    }
};

// Parse the value of the first keyword of a given list (terminal case of the recursion).
template<typename ...Ts>
struct _parse_values<0, Ts...> {
    void operator() (fitsfile* fptr, std::vector<std::string> keywords, std::tuple<Ts...>& values) {
        _parse_value(fptr, keywords[0], std::get<0>(values));
    }
};

}
/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
T parse_value(fitsfile* fptr, std::string keyword) {
    T value;
    int status = 0;
    fits_read_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, nullptr, &status);
    may_throw_cfitsio_error(status);
    return value;
}

template<typename ...Ts>
std::tuple<Ts...> parse_values(fitsfile* fptr, std::vector<std::string> keywords) {
    std::tuple<Ts...> values;
    internal::_parse_values<sizeof...(Ts)-1, Ts...>{}(fptr, keywords, values);
    return values;
}

template<typename T>
void write_value(fitsfile* fptr, std::string keyword, T value) {
    int status = 0;
    fits_write_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, nullptr, &status);
    may_throw_cfitsio_error(status);
}

template<>
void write_value<std::string>(fitsfile* fptr, std::string keyword, std::string value);

template<typename T>
void write_record(fitsfile* fptr, const record_type<T>& record) {
    const char* keyword = std::get<0>(record).c_str();
    const T& value = std::get<1>(record);
    const char* unit = std::get<2>(record).c_str();
    const char* comment = std::get<3>(record).c_str();
    int status = 0;
    fits_write_key(fptr, TypeCode<T>::for_record(), keyword, value, comment, &status);
    fits_write_key_unit(fptr, keyword, unit, &status);
    may_throw_cfitsio_error(status);
}

template<>
void write_record<std::string>(fitsfile* fptr, const record_type<std::string>& record);

template<typename T>
void update_value(fitsfile* fptr, std::string keyword, T value) {
    int status = 0;
    fits_update_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, nullptr, &status);
    may_throw_cfitsio_error(status);
}

template<>
void update_value<std::string>(fitsfile* fptr, std::string keyword, std::string value);

}
}

#endif

