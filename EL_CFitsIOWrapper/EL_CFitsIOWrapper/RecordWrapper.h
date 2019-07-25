/**
 * @file EL_CFitsIOWrapper/RecordHandler.h
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

#include "EL_CFitsIOWrapper/ErrorWrapper.h"
#include "EL_CFitsIOWrapper/TypeWrapper.h"

namespace Cfitsio {
namespace Record {

/**
 * Read the value of a given keyword with specified type.
 */
template<typename T>
T parse_value(fitsfile* fptr, std::string keyword);

/**
 * Read the values of a given set of keywords with specified types.
 */
template<typename... TRecords>
std::tuple<TRecords...> parse_values(fitsfile* fptr, std::vector<std::string> keywords);

/**
 * Read the value of a given keyword as a string.
 */
std::string read_value(fitsfile* fptr, std::string keyword);

/**
 * Read the values of a given set of keywords as a set of strings.
 */
std::vector<std::string> read_values(fitsfile* fptr, std::vector<std::string> keywords);


/////////////////////////
/// HELPER FUNCTIONS ///
///////////////////////


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


///////////////////////
/// IMPLEMENTATION ///
/////////////////////


template<typename T>
T parse_value(fitsfile* fptr, std::string keyword) {
    T value;
    int status = 0;
    fits_read_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, nullptr, &status);
    throw_cfitsio_error(status);
    return value;
}

template<>
std::string parse_value<std::string>(fitsfile* fptr, std::string keyword) {
    return read_value(fptr, keyword);
}

template<typename ...Ts>
std::tuple<Ts...> parse_values(fitsfile* fptr, std::vector<std::string> keywords) {
    std::tuple<Ts...> values;
    _parse_values<sizeof...(Ts)-1, Ts...>{}(fptr, keywords, values);
    return values;
}

}
}

#endif

