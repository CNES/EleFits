/**
 * @file src/lib/RecordHandler.cpp
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

#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Header {

template<>
FitsIO::Record<std::string> parse_record<std::string>(fitsfile* fptr, std::string keyword) {
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  if(length == 0)
    throw std::runtime_error("Cannot find keyword " + keyword);
  char* value = nullptr; // That's the only function in which CFitsIO allocates itself!
  char* unit = (char*) malloc(FLEN_COMMENT);
  char* comment = (char*) malloc(FLEN_COMMENT);
  fits_read_key_longstr(fptr, keyword.c_str(), &value, comment, &status);
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  FitsIO::Record<std::string> record(keyword, std::string(value), std::string(unit), std::string(comment));
  free(value);
  free(comment);
  free(unit);
  std::string context = "while parsing '" + keyword + "' in HDU #" + std::to_string(Hdu::current_index(fptr));
  may_throw_cfitsio_error(status, context);
  may_throw_cfitsio_error(status, context);
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
void write_record<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record) {
  int status = 0;
  if(record.value.length() > 68)
    fits_write_key_longwarn(fptr, &status);
  fits_write_key_longstr(fptr,
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  fits_write_key_unit(fptr, record.keyword.c_str(), record.unit.c_str(), &status);
  may_throw_cfitsio_error(status);
}

template<>
void update_record<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record) {
  int status = 0;
  fits_update_key(fptr,
      TypeCode<std::string>::for_record(),
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  may_throw_cfitsio_error(status);
}

void delete_record(fitsfile* fptr, std::string keyword) {
  int status = 0;
  fits_delete_key(fptr, keyword.c_str(), &status);
  may_throw_cfitsio_error(status);
}

}
}
}
