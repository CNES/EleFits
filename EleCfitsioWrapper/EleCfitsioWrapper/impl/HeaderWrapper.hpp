// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_HEADERWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"

#include <utility> // index_sequence, make_index_sequence

namespace Euclid {
namespace Cfitsio {
namespace HeaderIo {

/**
 * @copydoc parse_record
 */
template <>
Fits::Record<bool> parse_record<bool>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc parse_record
 */
template <>
Fits::Record<std::string> parse_record<std::string>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc parse_record
 */
template <>
Fits::Record<Fits::VariantValue> parse_record<Fits::VariantValue>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc write_record
 */
template <>
void write_record<bool>(fitsfile* fptr, const Fits::Record<bool>& record);

/**
 * @copydoc write_record
 */
template <>
void write_record<std::string>(fitsfile* fptr, const Fits::Record<std::string>& record);

/**
 * @copydoc write_record
 */
template <>
void write_record<const char*>(fitsfile* fptr, const Fits::Record<const char*>& record);

/**
 * @copydoc write_record
 */
template <>
void write_record<Fits::VariantValue>(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record);

/**
 * @copydoc update_record
 */
template <>
void update_record<bool>(fitsfile* fptr, const Fits::Record<bool>& record);

/**
 * @copydoc update_record
 */
template <>
void update_record<std::string>(fitsfile* fptr, const Fits::Record<std::string>& record);

/**
 * @copydoc update_record
 */
template <>
void update_record<const char*>(fitsfile* fptr, const Fits::Record<const char*>& record);

/**
 * @copydoc update_record
 */
template <>
void update_record<Fits::VariantValue>(fitsfile* fptr, const Fits::Record<Fits::VariantValue>& record);

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Use index_sequence to loop on keywords.
 */
template <class TReturn, typename... Ts, std::size_t... Is>
TReturn parse_records_as_impl(fitsfile* fptr, const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
  return {parse_record<Ts>(fptr, keywords[Is])...};
}

/**
 * @brief Use index_sequence to loop on records.
 */
template <typename... Ts, std::size_t... Is>
void write_records_impl(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records, std::index_sequence<Is...>) {
  using mock_unpack = int[];
  (void)mock_unpack {(write_record<Ts>(fptr, std::get<Is>(records)), 0)...};
}

/**
 * @brief Use index_sequence to loop on records.
 */
template <typename... Ts, std::size_t... Is>
void update_records_impl(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records, std::index_sequence<Is...>) {
  using mock_unpack = int[];
  (void)mock_unpack {(update_record<Ts>(fptr, std::get<Is>(records)), 0)...};
}

} // namespace Internal
/// @endcond

template <typename T>
Fits::Record<T> parse_record(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  /* Read value and comment */
  T value;
  char comment[FLEN_COMMENT];
  fits_read_key(fptr, TypeCode<T>::for_record(), keyword.c_str(), &value, comment, &status);
  /* Read unit */
  char unit[FLEN_COMMENT];
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  CfitsioError::may_throw(status, fptr, "Cannot parse record: " + keyword);
  /* Build Record */
  Fits::Record<T> record(keyword, value, std::string(unit), std::string(comment));
  /* Separate comment and unit */
  if (record.comment == record.unit) {
    record.comment = "";
  } else if (record.unit != "") {
    std::string match = "[" + record.unit + "] ";
    auto pos = record.comment.find(match);
    if (pos != std::string::npos) {
      record.comment.erase(pos, match.length());
    }
  }
  return record;
}

template <typename... Ts>
std::tuple<Fits::Record<Ts>...> parse_records(fitsfile* fptr, const std::vector<std::string>& keywords) {
  return parse_records_as<std::tuple<Fits::Record<Ts>...>, Ts...>(fptr, keywords);
}

template <class Return, typename... Ts>
Return parse_records_as(fitsfile* fptr, const std::vector<std::string>& keywords) {
  return Internal::parse_records_as_impl<Return, Ts...>(fptr, keywords, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
Fits::RecordVec<T> parse_record_vec(fitsfile* fptr, const std::vector<std::string>& keywords) {
  Fits::RecordVec<T> records(keywords.size());
  std::transform(keywords.begin(), keywords.end(), records.vector.begin(), [&](const std::string& k) {
    return parse_record<T>(fptr, k);
  });
  return records;
}

template <typename T>
void write_record(fitsfile* fptr, const Fits::Record<T>& record) {
  int status = 0;
  T nonconst_value = record.value;
  fits_write_key(
      fptr,
      TypeCode<T>::for_record(),
      record.keyword.c_str(),
      &nonconst_value,
      record.raw_comment().c_str(),
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot write record: " + record.keyword);
}

template <typename... Ts>
void write_records(fitsfile* fptr, const Fits::Record<Ts>&... records) {
  using mock_unpack = int[];
  (void)mock_unpack {(write_record(fptr, records), 0)...};
}

template <typename... Ts>
void write_records(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records) {
  Internal::write_records_impl(fptr, records, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
void write_records(fitsfile* fptr, const std::vector<Fits::Record<T>>& records) {
  for (const auto& r : records) {
    write_record(fptr, r);
  }
}

template <typename T>
void update_record(fitsfile* fptr, const Fits::Record<T>& record) {
  int status = 0;
  std::string comment = record.raw_comment();
  T value = record.value;
  fits_update_key(fptr, TypeCode<T>::for_record(), record.keyword.c_str(), &value, &comment[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot update record: " + record.keyword);
}

template <typename... Ts>
void update_records(fitsfile* fptr, const Fits::Record<Ts>&... records) {
  using mock_unpack = int[];
  (void)mock_unpack {(update_record(fptr, records), 0)...};
}

template <typename... Ts>
void update_records(fitsfile* fptr, const std::tuple<Fits::Record<Ts>...>& records) {
  Internal::update_records_impl(fptr, records, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
void update_records(fitsfile* fptr, const std::vector<Fits::Record<T>>& records) {
  for (const auto& r : records) {
    update_record(fptr, r);
  }
}

} // namespace HeaderIo
} // namespace Cfitsio
} // namespace Euclid

#endif
