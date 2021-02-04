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

#include <limits>

#include "EL_FitsData/FitsIOError.h"

#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Header {

std::string readHeader(fitsfile *fptr, bool incNonValued) {
  int status = 0;
  char *header = nullptr;
  int recordCount = 0;
  fits_hdr2str(
      fptr,
      not incNonValued,
      nullptr, // exclist => do not exclude any keyword
      0, // nexc => idem
      &header,
      &recordCount,
      &status);
  std::string headerString { header };
  fits_free_memory(header, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read the complete header");
  return headerString;
}

std::vector<std::string> listKeywords(fitsfile *fptr, FitsIO::KeywordCategory categories) {
  int count = 0;
  int status = 0;
  fits_get_hdrspace(fptr, &count, nullptr, &status);
  std::vector<std::string> keywords;
  char keyword[FLEN_KEYWORD];
  char value[FLEN_KEYWORD];
  for (int i = 0; i < count; ++i) {
    fits_read_keyn(fptr, i + 1, keyword, value, nullptr, &status);
    if (FitsIO::StandardKeyword::belongsCategories(keyword, categories)) {
      keywords.emplace_back(keyword);
    }
  }
  return keywords;
}

bool hasKeyword(fitsfile *fptr, const std::string &keyword) {
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  if (status == KEY_NO_EXIST) {
    return false;
  }
  CfitsioError::mayThrow(status, fptr, "Cannot check if record exists: " + keyword); // Other error codes
  return true; // No error
}

template <>
FitsIO::Record<bool> parseRecord<bool>(fitsfile *fptr, const std::string &keyword) { // TODO rm duplication
  int status = 0;
  /* Read value and comment */
  int nonconstIntValue; // TLOGICAL is for int in CFitsIO
  char comment[FLEN_COMMENT];
  fits_read_key(fptr, TypeCode<bool>::forRecord(), keyword.c_str(), &nonconstIntValue, comment, &status);
  /* Read unit */
  char unit[FLEN_COMMENT];
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot parse Boolean record: " + keyword);
  /* Build Record */
  FitsIO::Record<bool> record(keyword, nonconstIntValue, std::string(unit), std::string(comment));
  /* Separate comment and unit */
  if (record.comment == record.unit) {
    record.comment == "";
  } else if (record.unit != "") {
    std::string match = "[" + record.unit + "] ";
    auto pos = record.comment.find(match);
    if (pos != std::string::npos) {
      record.comment.erase(pos, match.length());
    }
  }
  return record;
}

template <>
FitsIO::Record<std::string>
parseRecord<std::string>(fitsfile *fptr, const std::string &keyword) { // TODO rm duplication
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot find string record: " + keyword);
  if (length == 0) {
    return { keyword, "" };
  }
  char *value = nullptr; // That's almost the only function in which CFitsIO allocates itself!
  char unit[FLEN_COMMENT];
  unit[0] = '\0';
  char comment[FLEN_COMMENT];
  comment[0] = '\0';
  fits_read_key_longstr(fptr, keyword.c_str(), &value, comment, &status);
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  std::string strValue(value);
  if (status == VALUE_UNDEFINED) {
    strValue = "";
    status = 0;
  }
  FitsIO::Record<std::string> record(keyword, strValue, std::string(unit), std::string(comment));
  free(value);
  CfitsioError::mayThrow(status, fptr, "Cannot parse string record: " + keyword);
  if (record.comment == record.unit) {
    record.comment == "";
  } else if (record.unit != "") {
    std::string match = "[" + record.unit + "] ";
    auto pos = record.comment.find(match);
    if (pos != std::string::npos) {
      record.comment.erase(pos, match.length());
    }
  }
  return record;
}

#define PARSE_RECORD_ANY_FOR_TYPE(type, unused) \
  if (id == typeid(type)) { \
    return FitsIO::Record<boost::any>(parseRecord<type>(fptr, keyword)); \
  }

template <>
FitsIO::Record<boost::any> parseRecord<boost::any>(fitsfile *fptr, const std::string &keyword) {
  const auto &id = recordTypeid(fptr, keyword);
  EL_FITSIO_FOREACH_RECORD_TYPE(PARSE_RECORD_ANY_FOR_TYPE)
  throw FitsIO::FitsIOError("Cannot deduce type for record: " + keyword);
}

template <>
void writeRecord<bool>(fitsfile *fptr, const FitsIO::Record<bool> &record) {
  int status = 0;
  int nonconstIntValue = record.value; // TLOGICAL is for int in CFitsIO
  fits_write_key(
      fptr,
      TypeCode<bool>::forRecord(),
      record.keyword.c_str(),
      &nonconstIntValue,
      record.rawComment().c_str(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write Boolean record: " + record.keyword);
}

template <>
void writeRecord<std::string>(fitsfile *fptr, const FitsIO::Record<std::string> &record) {
  int status = 0;
  if (record.hasLongStringValue()) { // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node118.html
    fits_write_key_longwarn(fptr, &status);
  }
  fits_write_key_longstr(fptr, record.keyword.c_str(), record.value.c_str(), record.rawComment().c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write string record: " + record.keyword);
}

template <>
void writeRecord<const char *>(fitsfile *fptr, const FitsIO::Record<const char *> &record) {
  int status = 0;
  if (record.hasLongStringValue()) { // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node118.html
    fits_write_key_longwarn(fptr, &status);
  }
  fits_write_key_longstr(fptr, record.keyword.c_str(), record.value, record.rawComment().c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write C string record: " + record.keyword);
}

template <typename T>
void writeRecordAnyImpl(fitsfile *fptr, const FitsIO::Record<boost::any> &record) {
  writeRecord<T>(fptr, { record.keyword, boost::any_cast<T>(record.value), record.unit, record.comment });
}

#define WRITE_RECORD_ANY_FOR_TYPE(type, unused) \
  if (id == typeid(type)) { \
    return writeRecordAnyImpl<type>(fptr, record); \
  }

template <>
void writeRecord<boost::any>(fitsfile *fptr, const FitsIO::Record<boost::any> &record) {
  const auto &id = record.value.type();
  EL_FITSIO_FOREACH_RECORD_TYPE(WRITE_RECORD_ANY_FOR_TYPE)
  WRITE_RECORD_ANY_FOR_TYPE(const char *, C_str)
  throw FitsIO::FitsIOError("Cannot deduce type for record: " + record.keyword);
}

template <>
void updateRecord<bool>(fitsfile *fptr, const FitsIO::Record<bool> &record) {
  int status = 0;
  std::string comment = record.comment;
  int nonconstIntValue = record.value; // TLOGICAL is for int in CFitsIO
  fits_update_key(fptr, TypeCode<bool>::forRecord(), record.keyword.c_str(), &nonconstIntValue, &comment[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot update Boolean record: " + record.keyword);
}

template <>
void updateRecord<std::string>(fitsfile *fptr, const FitsIO::Record<std::string> &record) {
  int status = 0;
  fits_update_key(
      fptr,
      TypeCode<std::string>::forRecord(),
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot update string record: " + record.keyword);
}

template <>
void updateRecord<const char *>(fitsfile *fptr, const FitsIO::Record<const char *> &record) {
  int status = 0;
  fits_update_key(
      fptr,
      TypeCode<std::string>::forRecord(),
      record.keyword.c_str(),
      &std::string(record.value)[0],
      &record.comment[0],
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot update C string record: " + record.keyword);
}

template <typename T>
void updateRecordAnyImpl(fitsfile *fptr, const FitsIO::Record<boost::any> &record) {
  updateRecord<T>(fptr, { record.keyword, boost::any_cast<T>(record.value), record.unit, record.comment });
}

#define UPDATE_RECORD_ANY_FOR_TYPE(type, unused) \
  if (id == typeid(type)) { \
    return updateRecordAnyImpl<type>(fptr, record); \
  }

template <>
void updateRecord<boost::any>(fitsfile *fptr, const FitsIO::Record<boost::any> &record) {
  const auto &id = record.value.type();
  EL_FITSIO_FOREACH_RECORD_TYPE(UPDATE_RECORD_ANY_FOR_TYPE)
  UPDATE_RECORD_ANY_FOR_TYPE(const char *, C_str)
  throw FitsIO::FitsIOError("Cannot deduce type for record: " + record.keyword);
}

void deleteRecord(fitsfile *fptr, const std::string &keyword) {
  int status = 0;
  fits_delete_key(fptr, keyword.c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot delete record: " + keyword);
}

namespace Internal {

/**
 * @brief Get the a typeid compatible with a negative integer value given as a string.
 */
const std::type_info &negIntRecordTypeidImpl(const std::string &value) {
  const long long parsed = std::stoll(value);
  if (parsed >= std::numeric_limits<char>::lowest()) {
    return typeid(char);
  }
  if (parsed >= std::numeric_limits<short>::lowest()) {
    return typeid(short);
  }
  if (parsed >= std::numeric_limits<int>::lowest()) {
    return typeid(int);
  }
  if (parsed >= std::numeric_limits<long>::lowest()) {
    return typeid(long);
  }
  return typeid(long long);
}

/**
 * @brief Get the a typeid compatible with a positive integer value given as a string.
 */
const std::type_info &posIntRecordTypeidImpl(const std::string &value) {
  const unsigned long long parsed = std::stoull(value);
  if (parsed <= std::numeric_limits<unsigned char>::max()) {
    return typeid(unsigned char);
  }
  if (parsed <= std::numeric_limits<unsigned short>::max()) {
    return typeid(unsigned short);
  }
  if (parsed <= std::numeric_limits<unsigned int>::max()) {
    return typeid(unsigned int);
  }
  if (parsed <= std::numeric_limits<unsigned long>::max()) {
    return typeid(unsigned long);
  }
  return typeid(unsigned long long);
}

/**
 * @brief Get the a typeid compatible with an integer value given as a string.
 * @return The typeid of the smallest signed (resp. unsigned) int type which can accommodate the value
 * if the first character is (resp. is not) '-'.
 */
const std::type_info &intRecordTypeidImpl(const std::string &value) {
  return (value[0] == '-') ? negIntRecordTypeidImpl(value) : posIntRecordTypeidImpl(value);
}

/**
 * @return typeid(float) if in (lowest(float), max(float)); typeid(double) otherwise.
 */
const std::type_info &floatRecordTypeidImpl(const std::string &value) {
  double parsed = std::stod(value);
  if (parsed < std::numeric_limits<float>::lowest()) {
    return typeid(double);
  }
  if (parsed > std::numeric_limits<float>::max()) {
    return typeid(double);
  }
  return typeid(float);
}

/**
 * @return typeid(std::complex<float>) if both real and imaginary parts are in (lowest(float), max(float));
 * typeid(std::complex<double>) otherwise.
 */
const std::type_info &complexRecordTypeidImpl(const std::string &value) {
  const std::size_t reBegin = 1; // 1 for '('
  const std::size_t reEnd = value.find(",");
  const std::size_t imBegin = reEnd + 2; // 2 for ", "
  const std::size_t imEnd = value.find(")");
  if (reEnd == std::string::npos || imEnd == std::string::npos) {
    throw FitsIO::FitsIOError("Cannot parse complex value: " + value);
  }
  const std::string re = value.substr(reBegin, reEnd - reBegin);
  if (floatRecordTypeidImpl(re) == typeid(double)) {
    return typeid(std::complex<double>);
  }
  const std::string im = value.substr(imBegin, imEnd - imBegin);
  if (floatRecordTypeidImpl(im) == typeid(double)) {
    return typeid(std::complex<double>);
  }
  return typeid(std::complex<float>);
}

} // namespace Internal

/**
 * @see https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node52.html
 */
const std::type_info &recordTypeid(fitsfile *fptr, const std::string &keyword) {
  int status = 0;
  char value[FLEN_VALUE];
  auto nonconstKeyword = keyword;
  fits_read_keyword(fptr, &keyword[0], value, nullptr, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot read record: " + keyword);
  char dtype = ' ';
  fits_get_keytype(value, &dtype, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot deduce type code of record: " + keyword);
  // 'C', 'L', 'I', 'F' or 'X', for character string, logical, integer, floating point, or complex
  switch (dtype) {
    case 'C':
      return typeid(std::string);
    case 'L':
      return typeid(bool);
    case 'I':
      return Internal::intRecordTypeidImpl(value);
    case 'F':
      return Internal::floatRecordTypeidImpl(value);
    case 'X':
      return Internal::complexRecordTypeidImpl(value);
    default:
      throw FitsIO::FitsIOError("Cannot deduce type code of record: " + keyword);
  }
}

void writeComment(fitsfile *fptr, const std::string &comment) {
  int status = 0;
  std::string nonconstComment = comment;
  fits_write_comment(fptr, &nonconstComment[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write COMMENT record");
}

void writeHistory(fitsfile *fptr, const std::string &history) {
  int status = 0;
  std::string nonconstHistory = history;
  fits_write_history(fptr, &nonconstHistory[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write HISTORY record");
}

} // namespace Header
} // namespace Cfitsio
} // namespace Euclid
