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

#ifndef _EL_CFITSIOWRAPPER_RECORDHANDLER_H
#define _EL_CFITSIOWRAPPER_RECORDHANDLER_H

#include <fitsio.h>
#include <string>
#include <tuple>
#include <vector>

#include "EL_FitsData/Record.h"

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Header-related functions.
 */
namespace Header {

/**
 * @brief List all the keywords.
 */
std::vector<std::string> list_keywords(fitsfile* fptr);

/**
 * @brief Parse a record.
 */
template<typename T>
FitsIO::Record<T> parse_record(fitsfile* fptr, std::string keyword);

/**
 * @brief Parse records.
 */
template<typename... Ts>
std::tuple<FitsIO::Record<Ts>...> parse_records(fitsfile* fptr, const std::vector<std::string>& keywords);

/**
 * @brief Parse records and store them in a user-defined structure.
 * @tparam Return A class which can be brace-initialized with a pack of records or values.
 */
template<class Return, typename... Ts>
Return parse_records_as(fitsfile* fptr, const std::vector<std::string>& keywords);

/**
 * @brief Write a new record.
 */
template<typename T>
void write_record(fitsfile* fptr, const FitsIO::Record<T>& record);

/**
 * @brief Write new records.
 */
template<typename... Ts>
void write_records(fitsfile* fptr, const FitsIO::Record<Ts>&... records);

/**
 * @brief Update an existing record or write a new one.
 */
template<typename T>
void update_record(fitsfile* fptr, const FitsIO::Record<T>& record);

/**
 * @brief Update existing records or write new ones.
 */
template<typename... Ts>
void update_records(fitsfile* fptr, const FitsIO::Record<Ts>&... records);

/**
 * @brief Delete an existing record.
 */
void delete_record(fitsfile* fptr, std::string keyword);


///////////////
// INTENRAL //
/////////////


/// @cond INTERNAL
namespace internal {

// Signature change (output argument) for further use with variadic templates.
template<typename T>
inline void _parse_record(fitsfile* fptr, std::string keyword, FitsIO::Record<T>& record) {
  record = parse_record<T>(fptr, keyword);
}

// Parse the records of the i+1 first keywords of a given list (recursive approach).
template<int i, typename ...Ts>
struct _parse_records {
  void operator() (fitsfile* fptr, std::vector<std::string> keywords, std::tuple<FitsIO::Record<Ts>...>& records) {
    _parse_record(fptr, keywords[i], std::get<i>(records));
    _parse_records<i-1, Ts...>{}(fptr, keywords, records);
  }
};

// Parse the value of the first keyword of a given list (terminal case of the recursion).
template<typename ...Ts>
struct _parse_records<0, Ts...> {
  void operator() (fitsfile* fptr, std::vector<std::string> keywords, std::tuple<FitsIO::Record<Ts>...>& records) {
    _parse_record(fptr, keywords[0], std::get<0>(records));
  }
};

template<class Return, typename... Ts, std::size_t... Is>
Return _parse_records_as(fitsfile* fptr, const std::vector<std::string>& keywords, std14::index_sequence<Is...>) {
  return { parse_record<Ts>(fptr, keywords[Is]) ... };
}

}
/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
FitsIO::Record<T> parse_record(fitsfile* fptr, std::string keyword) {
  int status = 0;
  /* Read value and comment */
  T value;
  char comment[FLEN_COMMENT];
  comment[0] = '\0';
  fits_read_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, comment, &status);
  /* Read unit */
  char unit[FLEN_COMMENT];
  unit[0] = '\0';
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  std::string context = "while parsing '" + keyword + "' in HDU #" + std::to_string(Hdu::current_index(fptr));
  may_throw_cfitsio_error(status, context);
  /* Build Record */
  FitsIO::Record<T> record(keyword, value, std::string(unit), std::string(comment));
  /* Separate comment and unit */
  if(record.comment == record.unit) {
    record.comment == "";
  } else if(record.unit != "") {
    std::string match = "[" + record.unit + "] ";
    auto pos = record.comment.find(match);
    if(pos != std::string::npos)
      record.comment.erase(pos, match.length());
  }
  return record;
}

template<>
FitsIO::Record<std::string> parse_record<std::string>(fitsfile* fptr, std::string keyword);

template<typename ...Ts>
std::tuple<FitsIO::Record<Ts>...> parse_records(fitsfile* fptr, const std::vector<std::string>& keywords) {
  std::tuple<FitsIO::Record<Ts>...> records;
  internal::_parse_records<sizeof...(Ts)-1, Ts...>{}(fptr, keywords, records);
  return records;
}

template<class Return, typename... Ts>
Return parse_records_as(fitsfile* fptr, const std::vector<std::string>& keywords) {
  return internal::_parse_records_as<Return, Ts...>(fptr, keywords, std14::make_index_sequence<sizeof...(Ts)>());
}

template<typename T>
void write_record(fitsfile* fptr, const FitsIO::Record<T>& record) {
  int status = 0;
  std::string comment = record.comment;
  T value = record.value;
  fits_write_key(fptr, TypeCode<T>::for_record(), record.keyword.c_str(), &value, &comment[0], &status);
  fits_write_key_unit(fptr, record.keyword.c_str(), record.unit.c_str(), &status);
  std::string context = "while writing '" + record.keyword + "' in HDU #" + std::to_string(Hdu::current_index(fptr));
  may_throw_cfitsio_error(status, context);
}

template<>
void write_record<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record);

template<typename... Ts>
void write_records(fitsfile* fptr, const FitsIO::Record<Ts>&... records) {
  using mock_unpack = int[];
  (void)mock_unpack {(write_record(fptr, records), 0)...};
}

template<typename T>
void update_record(fitsfile* fptr, const FitsIO::Record<T>& record) {
  int status = 0;
  std::string comment = record.comment;
  T value = record.value;
  fits_update_key(fptr, TypeCode<T>::for_record(), record.keyword.c_str(), &value, &comment[0], &status);
  std::string context = "while updating '" + record.keyword + "' in HDU #" + std::to_string(Hdu::current_index(fptr));
  may_throw_cfitsio_error(status, context);
}

template<>
void update_record<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record);

}
}
}

#endif
