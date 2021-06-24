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

#if defined(_EL_CFITSIOWRAPPER_HEADERWRAPPER_IMPL) || defined(CHECK_QUALITY)

  #include "EL_CfitsioWrapper/HeaderWrapper.h"

  #include <utility> // index_sequence, make_index_sequence

namespace Euclid {
namespace Cfitsio {
namespace Header {

/**
 * @copydoc parseRecord
 */
template <>
FitsIO::Record<bool> parseRecord<bool>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc parseRecord
 */
template <>
FitsIO::Record<std::string> parseRecord<std::string>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc parseRecord
 */
template <>
FitsIO::Record<FitsIO::VariantValue> parseRecord<FitsIO::VariantValue>(fitsfile* fptr, const std::string& keyword);

/**
 * @copydoc writeRecord
 */
template <>
void writeRecord<bool>(fitsfile* fptr, const FitsIO::Record<bool>& record);

/**
 * @copydoc writeRecord
 */
template <>
void writeRecord<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record);

/**
 * @copydoc writeRecord
 */
template <>
void writeRecord<const char*>(fitsfile* fptr, const FitsIO::Record<const char*>& record);

/**
 * @copydoc writeRecord
 */
template <>
void writeRecord<FitsIO::VariantValue>(fitsfile* fptr, const FitsIO::Record<FitsIO::VariantValue>& record);

/**
 * @copydoc updateRecord
 */
template <>
void updateRecord<bool>(fitsfile* fptr, const FitsIO::Record<bool>& record);

/**
 * @copydoc updateRecord
 */
template <>
void updateRecord<std::string>(fitsfile* fptr, const FitsIO::Record<std::string>& record);

/**
 * @copydoc updateRecord
 */
template <>
void updateRecord<const char*>(fitsfile* fptr, const FitsIO::Record<const char*>& record);

/**
 * @copydoc updateRecord
 */
template <>
void updateRecord<FitsIO::VariantValue>(fitsfile* fptr, const FitsIO::Record<FitsIO::VariantValue>& record);

/// @cond INTERNAL
namespace Internal {

/**
 * @brief Use index_sequence to loop on keywords.
 */
template <class TReturn, typename... Ts, std::size_t... Is>
TReturn parseRecordsAsImpl(fitsfile* fptr, const std::vector<std::string>& keywords, std::index_sequence<Is...>) {
  return { parseRecord<Ts>(fptr, keywords[Is])... };
}

/**
 * @brief Use index_sequence to loop on records.
 */
template <typename... Ts, std::size_t... Is>
void writeRecordsImpl(fitsfile* fptr, const std::tuple<FitsIO::Record<Ts>...>& records, std::index_sequence<Is...>) {
  using mockUnpack = int[];
  (void)mockUnpack { (writeRecord<Ts>(fptr, std::get<Is>(records)), 0)... };
}

/**
 * @brief Use index_sequence to loop on records.
 */
template <typename... Ts, std::size_t... Is>
void updateRecordsImpl(fitsfile* fptr, const std::tuple<FitsIO::Record<Ts>...>& records, std::index_sequence<Is...>) {
  using mockUnpack = int[];
  (void)mockUnpack { (updateRecord<Ts>(fptr, std::get<Is>(records)), 0)... };
}

} // namespace Internal
/// @endcond

template <typename T>
FitsIO::Record<T> parseRecord(fitsfile* fptr, const std::string& keyword) {
  int status = 0;
  /* Read value and comment */
  T value;
  char comment[FLEN_COMMENT];
  fits_read_key(fptr, TypeCode<T>::forRecord(), keyword.c_str(), &value, comment, &status);
  /* Read unit */
  char unit[FLEN_COMMENT];
  fits_read_key_unit(fptr, keyword.c_str(), unit, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot parse record: " + keyword);
  /* Build Record */
  FitsIO::Record<T> record(keyword, value, std::string(unit), std::string(comment));
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

template <typename... Ts>
std::tuple<FitsIO::Record<Ts>...> parseRecords(fitsfile* fptr, const std::vector<std::string>& keywords) {
  return parseRecordsAs<std::tuple<FitsIO::Record<Ts>...>, Ts...>(fptr, keywords);
}

template <class Return, typename... Ts>
Return parseRecordsAs(fitsfile* fptr, const std::vector<std::string>& keywords) {
  return Internal::parseRecordsAsImpl<Return, Ts...>(fptr, keywords, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
FitsIO::RecordVector<T> parseRecordVector(fitsfile* fptr, const std::vector<std::string>& keywords) {
  FitsIO::RecordVector<T> records(keywords.size());
  std::transform(keywords.begin(), keywords.end(), records.vector.begin(), [&](const std::string& k) {
    return parseRecord<T>(fptr, k);
  });
  return records;
}

template <typename T>
void writeRecord(fitsfile* fptr, const FitsIO::Record<T>& record) {
  int status = 0;
  T nonconstValue = record.value;
  fits_write_key(
      fptr,
      TypeCode<T>::forRecord(),
      record.keyword.c_str(),
      &nonconstValue,
      record.rawComment().c_str(),
      &status);
  CfitsioError::mayThrow(status, fptr, "Cannot write record: " + record.keyword);
}

template <typename... Ts>
void writeRecords(fitsfile* fptr, const FitsIO::Record<Ts>&... records) {
  using mockUnpack = int[];
  (void)mockUnpack { (writeRecord(fptr, records), 0)... };
}

template <typename... Ts>
void writeRecords(fitsfile* fptr, const std::tuple<FitsIO::Record<Ts>...>& records) {
  Internal::writeRecordsImpl(fptr, records, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
void writeRecords(fitsfile* fptr, const std::vector<FitsIO::Record<T>>& records) {
  for (const auto& r : records) {
    writeRecord(fptr, r);
  }
}

template <typename T>
void updateRecord(fitsfile* fptr, const FitsIO::Record<T>& record) {
  int status = 0;
  std::string comment = record.comment;
  T value = record.value;
  fits_update_key(fptr, TypeCode<T>::forRecord(), record.keyword.c_str(), &value, &comment[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot update record: " + record.keyword);
}

template <typename... Ts>
void updateRecords(fitsfile* fptr, const FitsIO::Record<Ts>&... records) {
  using mockUnpack = int[];
  (void)mockUnpack { (updateRecord(fptr, records), 0)... };
}

template <typename... Ts>
void updateRecords(fitsfile* fptr, const std::tuple<FitsIO::Record<Ts>...>& records) {
  Internal::updateRecordsImpl(fptr, records, std::make_index_sequence<sizeof...(Ts)>());
}

template <typename T>
void updateRecords(fitsfile* fptr, const std::vector<FitsIO::Record<T>>& records) {
  for (const auto& r : records) {
    updateRecord(fptr, r);
  }
}

} // namespace Header
} // namespace Cfitsio
} // namespace Euclid

#endif
