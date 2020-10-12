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

#ifdef _EL_FITSFILE_HDU_IMPL

#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
Record<T> RecordHdu::parseRecord(const std::string &keyword) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecord<T>(m_fptr, keyword);
}

template <typename T>
Record<T> RecordHdu::parseRecordOr(const Record<T> &fallback) const {
  gotoThisHdu();
  if (hasKeyword(fallback.keyword)) {
    return parseRecord<T>(fallback.keyword);
  }
  return fallback;
}

template <typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parseRecords(const std::vector<std::string> &keywords) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecords<Ts...>(m_fptr, keywords);
}

template <class Return, typename... Ts>
Return RecordHdu::parseRecordsAs(const std::vector<std::string> &keywords) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecordsAs<Return, Ts...>(m_fptr, keywords);
}

template <typename T>
RecordVector<T> RecordHdu::parseRecordVector(const std::vector<std::string> &keywords) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecordVector<T>(m_fptr, keywords);
}

template <typename T>
RecordVector<T> RecordHdu::parseAllRecords() const {
  return parseRecordVector<T>(readKeywords());
}

template <typename T>
void RecordHdu::writeRecord(const Record<T> &record) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecord(m_fptr, record);
}

template <typename T>
void RecordHdu::writeRecord(const std::string &k, T v, const std::string &u, const std::string &c) const {
  writeRecord(Record<T>(k, v, u, c));
}

template <typename... Ts>
void RecordHdu::writeRecords(const Record<Ts> &... records) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecords(m_fptr, records...);
}

template <typename... Ts>
void RecordHdu::writeRecords(const std::tuple<Record<Ts>...> &records) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecords(m_fptr, records);
}

template <typename T>
void RecordHdu::writeRecords(const std::vector<Record<T>> &records) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecords(m_fptr, records);
}

template <typename T>
void RecordHdu::writeRecords(const RecordVector<T> &records, const std::vector<std::string> &keywords) const {
  gotoThisHdu();
  for (const auto &k : keywords) {
    Cfitsio::Header::writeRecord(m_fptr, records[k]);
  }
}

template <typename T>
void RecordHdu::updateRecord(const Record<T> &record) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecord(m_fptr, record);
}

template <typename T>
void RecordHdu::updateRecord(const std::string &k, T v, const std::string &u, const std::string &c) const {
  updateRecord(Record<T>(k, v, u, c));
}

template <typename... Ts>
void RecordHdu::updateRecords(const Record<Ts> &... records) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecords(m_fptr, records...);
}

template <typename... Ts>
void RecordHdu::updateRecords(const std::tuple<Record<Ts>...> &records) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecords(m_fptr, records);
}

template <typename T>
void RecordHdu::updateRecords(const std::vector<Record<T>> &records) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecords(m_fptr, records);
}

template <typename T>
void RecordHdu::updateRecords(const RecordVector<T> &records, const std::vector<std::string> &keywords) const {
  gotoThisHdu();
  for (const auto &k : keywords) {
    Cfitsio::Header::updateRecord(m_fptr, records[k]);
  }
}

#ifndef DECLARE_PARSE_RECORD
#define DECLARE_PARSE_RECORD(type, unused) \
  extern template Record<type> RecordHdu::parseRecord(const std::string &) const;
EL_FITSIO_FOREACH_RECORD_TYPE(DECLARE_PARSE_RECORD)
#undef DECLARE_PARSE_RECORD
#endif

extern template RecordVector<boost::any> RecordHdu::parseRecordVector(const std::vector<std::string> &) const;

#ifndef DECLARE_WRITE_RECORD
#define DECLARE_WRITE_RECORD(type, unused) \
  extern template void RecordHdu::writeRecord(const Record<type> &) const; \
  extern template void RecordHdu::updateRecord(const Record<type> &) const;
EL_FITSIO_FOREACH_RECORD_TYPE(DECLARE_WRITE_RECORD)
#undef DECLARE_WRITE_RECORD
#endif

extern template void RecordHdu::writeRecords(const std::vector<Record<boost::any>> &) const;
extern template void RecordHdu::updateRecords(const std::vector<Record<boost::any>> &) const;

} // namespace FitsIO
} // namespace Euclid

#endif
