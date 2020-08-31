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

#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Header {

std::vector<std::string> listKeywords(fitsfile* fptr) {
  int count = 0;
  int status = 0;
  fits_get_hdrspace(fptr, &count, nullptr, &status);
  std::vector<std::string> keywords(count);
  char keyword[FLEN_KEYWORD];
  char value[FLEN_KEYWORD];
  for(int i=0; i<count; ++i) {
    fits_read_keyn(fptr, i+1, keyword, value, nullptr, &status);
    keywords[i].assign(keyword);
  }
  return keywords;
}

template<>
FitsIO::Record<std::string> parseRecord<std::string>(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  mayThrowCfitsioError(status, "Cannot find string keyword " + keyword);
  if(length == 0)
    return { keyword, "" };
  char* value = nullptr; // That's the only function in which CFitsIO allocates itself!
  char unit[FLEN_COMMENT];
  unit[0] = '\0';
  char comment[FLEN_COMMENT];
  comment[0] = '\0';
  fits_read_key_longstr(fptr, keyword.c_str(), &value, comment, &status);
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  std::string str_value(value);
  if(status == VALUE_UNDEFINED) {
    str_value = "";
    status = 0;
  }
  FitsIO::Record<std::string> record(keyword, str_value, std::string(unit), std::string(comment));
  free(value);
  std::string context = "while parsing '" + keyword + "' in HDU #" + std::to_string(Hdu::currentIndex(fptr));
  mayThrowCfitsioError(status, context);
  mayThrowCfitsioError(status, context);
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
void writeRecord<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record) {
  int status = 0;
  if(record.value.length() > 68) // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node118.html
    fits_write_key_longwarn(fptr, &status);
  fits_write_key_longstr(fptr,
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  fits_write_key_unit(fptr, record.keyword.c_str(), record.unit.c_str(), &status);
  mayThrowCfitsioError(status);
}

template<>
void writeRecord<const char*>(fitsfile* fptr, const FitsIO::Record<const char*>& record) {
  int status = 0;
  if(strlen(record.value) > 68) // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node118.html
    fits_write_key_longwarn(fptr, &status);
  fits_write_key_longstr(fptr,
      record.keyword.c_str(),
      record.value,
      &record.comment[0],
      &status);
  fits_write_key_unit(fptr, record.keyword.c_str(), record.unit.c_str(), &status);
  mayThrowCfitsioError(status);
}

template<>
void updateRecord<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record) {
  int status = 0;
  fits_update_key(fptr,
      TypeCode<std::string>::forRecord(),
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  mayThrowCfitsioError(status);
}

template<>
void updateRecord<const char*>(fitsfile* fptr, const FitsIO::Record<const char*>& record) {
  int status = 0;
  fits_update_key(fptr,
      TypeCode<std::string>::forRecord(),
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  mayThrowCfitsioError(status);
}

void deleteRecord(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  fits_delete_key(fptr, keyword.c_str(), &status);
  mayThrowCfitsioError(status);
}

}
}
}
