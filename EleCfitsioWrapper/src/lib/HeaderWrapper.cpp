// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/HeaderWrapper.h"

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleFitsData/FitsError.h"

#include <limits>

namespace Euclid {
namespace Cfitsio {
namespace HeaderIo {

std::string read_header(fitsfile* fptr, bool inc_non_valued) {
  int status = 0;
  char* header = nullptr;
  int record_count = 0;
  fits_hdr2str(
      fptr,
      not inc_non_valued,
      nullptr, // exclist => do not exclude any keyword
      0, // nexc => idem
      &header,
      &record_count,
      &status);
  std::string header_htring {header};
  fits_free_memory(header, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read the complete header");
  return header_htring;
}

std::vector<std::string> list_keywords(fitsfile* fptr, Fits::KeywordCategory categories) {
  int count = 0;
  int status = 0;
  fits_get_hdrspace(fptr, &count, nullptr, &status);
  std::vector<std::string> keywords; // TODO Should we reserve(count) while categories can filter a lot?
  char keyword[FLEN_KEYWORD];
  char value[FLEN_KEYWORD];
  for (int i = 0; i < count; ++i) {
    fits_read_keyn(fptr, i + 1, keyword, value, nullptr, &status);
    if (Fits::KeywordCategory::belongsCategories(keyword, categories)) {
      keywords.emplace_back(keyword);
    }
  }
  return keywords;
}

std::map<std::string, std::string> list_keywords_values(fitsfile* fptr, Fits::KeywordCategory categories) {
  int count = 0;
  int status = 0;
  fits_get_hdrspace(fptr, &count, nullptr, &status);
  std::map<std::string, std::string> records;
  char keyword[FLEN_KEYWORD];
  char value[FLEN_KEYWORD];
  for (int i = 0; i < count; ++i) {
    fits_read_keyn(fptr, i + 1, keyword, value, nullptr, &status);
    if (Fits::KeywordCategory::belongsCategories(keyword, categories)) {
      records[keyword] = value;
    }
  }
  return records;
}

bool has_keyword(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  if (status == KEY_NO_EXIST) {
    return false;
  }
  CfitsioError::may_throw(status, fptr, "Cannot check if record exists: " + keyword); // Other error codes
  return true; // No error
}

template <>
Fits::Record<bool> parse_record<bool>(fitsfile* fptr, const std::string& keyword) { // TODO rm duplication
  int status = 0;
  /* Read value and comment */
  int nonconst_int_value; // TLOGICAL is for int in CFITSIO
  char comment[FLEN_COMMENT];
  fits_read_key(fptr, TypeCode<bool>::for_record(), keyword.c_str(), &nonconst_int_value, comment, &status);
  /* Read unit */
  char unit[FLEN_COMMENT];
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  CfitsioError::may_throw(status, fptr, "Cannot parse Boolean record: " + keyword);
  /* Build Record */
  Fits::Record<bool> record(keyword, nonconst_int_value, std::string(unit), std::string(comment));
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
Fits::Record<std::string> parse_record<std::string>(fitsfile* fptr, const std::string& keyword) {
  // TODO rm duplication
  int status = 0;
  int length = 0;
  fits_get_key_strlen(fptr, keyword.c_str(), &length, &status);
  CfitsioError::may_throw(status, fptr, "Cannot find string record: " + keyword);
  if (length == 0) {
    return {keyword, ""};
  }
  char* value = nullptr; // That's almost the only function in which CFITSIO allocates itself!
  char unit[FLEN_COMMENT];
  unit[0] = '\0';
  char comment[FLEN_COMMENT];
  // FIXME Could be longer! No known way of reading the comment size (mail sent)
  // Option: don't read comment if length > 68
  comment[0] = '\0';
  fits_read_key_longstr(fptr, keyword.c_str(), &value, comment, &status);
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  std::string strValue(value);
  if (status == VALUE_UNDEFINED) {
    strValue = "";
    status = 0;
  }
  Fits::Record<std::string> record(keyword, strValue, std::string(unit), std::string(comment));
  free(value);
  CfitsioError::may_throw(status, fptr, "Cannot parse string record: " + keyword);
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
    return Fits::Record<Fits::VariantValue>(parse_record<type>(fptr, keyword)); \
  }

template <>
Fits::Record<Fits::VariantValue> parse_record<Fits::VariantValue>(fitsfile* fptr, const std::string& keyword) {
  const auto& id = record_typeid(fptr, keyword);
  ELEFITS_FOREACH_RECORD_TYPE(PARSE_RECORD_ANY_FOR_TYPE)
  throw Fits::FitsError("Cannot deduce type for record: " + keyword);
}

template <>
void write_record<bool>(fitsfile* fptr, const Fits::Record<bool>& record) {
  int status = 0;
  int nonconst_int_value = record.value; // TLOGICAL is for int in CFITSIO
  fits_write_key(
      fptr,
      TypeCode<bool>::for_record(),
      record.keyword.c_str(),
      &nonconst_int_value,
      record.rawComment().c_str(),
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot write Boolean record: " + record.keyword);
}

template <>
void write_record<std::string>(fitsfile* fptr, const Fits::Record<std::string>& record) {
  int status = 0;
  if (record.hasLongStringValue()) { // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node118.html
    fits_write_key_longwarn(fptr, &status);
  }
  fits_write_key_longstr(fptr, record.keyword.c_str(), record.value.c_str(), record.rawComment().c_str(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot write string record: " + record.keyword);
}

template <>
void write_record<const char*>(fitsfile* fptr, const Fits::Record<const char*>& record) {
  write_record<std::string>(fptr, {record.keyword, std::string(record.value), record.unit, record.comment});
}

template <typename T>
void write_record_any_impl(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record) {
  write_record<T>(fptr, {record.keyword, boost::any_cast<T>(record.value), record.unit, record.comment});
}

#define WRITE_RECORD_ANY_FOR_TYPE(type, unused) \
  if (id == typeid(type)) { \
    return write_record_any_impl<type>(fptr, record); \
  }

template <>
void write_record<Fits::VariantValue>(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record) {
  const auto& id = record.value.type();
  ELEFITS_FOREACH_RECORD_TYPE(WRITE_RECORD_ANY_FOR_TYPE)
  WRITE_RECORD_ANY_FOR_TYPE(const char*, C_str)
  throw Fits::FitsError("Cannot deduce type for record: " + record.keyword);
}

template <>
void update_record<bool>(fitsfile* fptr, const Fits::Record<bool>& record) {
  int status = 0;
  std::string comment = record.rawComment();
  int nonconst_int_value = record.value; // TLOGICAL is for int in CFITSIO
  fits_update_key(fptr, TypeCode<bool>::for_record(), record.keyword.c_str(), &nonconst_int_value, &comment[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot update Boolean record: " + record.keyword);
}

template <>
void update_record<std::string>(fitsfile* fptr, const Fits::Record<std::string>& record) {
  if (record.hasLongStringValue()) { // Cannot use fits_update_key for long string records
    if (has_keyword(fptr, record)) {
      remove_record(fptr, record.keyword);
    }
    write_record(fptr, record);
    // Keyword ordering is changed after deletion, but there is no better (simple) option
  } else {
    int status = 0;
    std::string comment = record.rawComment();
    fits_update_key(
        fptr,
        TypeCode<std::string>::for_record(),
        record.keyword.c_str(),
        &std::string(record.value)[0],
        &comment[0],
        &status);
    CfitsioError::may_throw(status, fptr, "Cannot update string record: " + record.keyword);
  }
}

template <>
void update_record<const char*>(fitsfile* fptr, const Fits::Record<const char*>& record) {
  update_record<std::string>(fptr, {record.keyword, std::string(record.value), record.unit, record.comment});
}

template <typename T>
void update_record_any_impl(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record) {
  update_record<T>(fptr, {record.keyword, boost::any_cast<T>(record.value), record.unit, record.comment});
}

#define UPDATE_RECORD_ANY_FOR_TYPE(type, unused) \
  if (id == typeid(type)) { \
    return update_record_any_impl<type>(fptr, record); \
  }

template <>
void update_record<Fits::VariantValue>(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record) {
  const auto& id = record.value.type();
  ELEFITS_FOREACH_RECORD_TYPE(UPDATE_RECORD_ANY_FOR_TYPE)
  UPDATE_RECORD_ANY_FOR_TYPE(const char*, C_str)
  throw Fits::FitsError("Cannot deduce type for record: " + record.keyword);
}

void remove_record(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  fits_delete_key(fptr, keyword.c_str(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot delete record: " + keyword);
}

namespace Internal {

/**
 * @brief Get the a typeid compatible with a negative integer value given as a string.
 */
const std::type_info& neg_int_record_typeid_impl(const std::string& value) {
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
const std::type_info& pos_int_record_typeid_impl(const std::string& value) {
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
const std::type_info& int_record_typeid_impl(const std::string& value) {
  return (value[0] == '-') ? neg_int_record_typeid_impl(value) : pos_int_record_typeid_impl(value);
}

/**
 * @return typeid(float) if in (lowest(float), max(float)); typeid(double) otherwise.
 */
const std::type_info& float_record_typeid_impl(const std::string& value) {
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
const std::type_info& complex_record_typeid_impl(const std::string& value) {
  const std::size_t re_begin = 1; // 1 for '('
  const std::size_t re_end = value.find(",");
  const std::size_t im_begin = re_end + 2; // 2 for ", "
  const std::size_t im_end = value.find(")");
  if (re_end == std::string::npos || im_end == std::string::npos) {
    throw Fits::FitsError("Cannot parse complex value: " + value);
  }
  const std::string re = value.substr(re_begin, re_end - re_begin);
  if (float_record_typeid_impl(re) == typeid(double)) {
    return typeid(std::complex<double>);
  }
  const std::string im = value.substr(im_begin, im_end - im_begin);
  if (float_record_typeid_impl(im) == typeid(double)) {
    return typeid(std::complex<double>);
  }
  return typeid(std::complex<float>);
}

} // namespace Internal

/**
 * @see https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node52.html
 */
const std::type_info& record_typeid(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  char value[FLEN_VALUE];
  fits_read_keyword(fptr, &keyword[0], value, nullptr, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read record: " + keyword);
  char dtype = ' ';
  fits_get_keytype(value, &dtype, &status);
  CfitsioError::may_throw(status, fptr, "Cannot deduce type code of record: " + keyword);
  // 'C', 'L', 'I', 'F' or 'X', for character string, logical, integer, floating point, or complex
  switch (dtype) {
    case 'C':
      return typeid(std::string);
    case 'L':
      return typeid(bool);
    case 'I':
      return Internal::int_record_typeid_impl(value);
    case 'F':
      return Internal::float_record_typeid_impl(value);
    case 'X':
      return Internal::complex_record_typeid_impl(value);
    default:
      throw Fits::FitsError("Cannot deduce type code of record: " + keyword);
  }
}

void write_comment(fitsfile* fptr, const std::string& comment) {
  int status = 0;
  std::string nonconst_comment = comment;
  fits_write_comment(fptr, &nonconst_comment[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot write COMMENT record");
}

void write_history(fitsfile* fptr, const std::string& history) {
  int status = 0;
  std::string nonconst_history = history;
  fits_write_history(fptr, &nonconst_history[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot write HISTORY record");
}

} // namespace HeaderIo
} // namespace Cfitsio
} // namespace Euclid
