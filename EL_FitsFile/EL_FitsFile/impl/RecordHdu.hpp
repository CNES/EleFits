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

#if defined(_EL_FITSFILE_RECORDHDU_IMPL) || defined(CHECK_QUALITY)

  #include "EL_CfitsioWrapper/HeaderWrapper.h"
  #include "EL_FitsData/RecordVector.h"
  #include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
const T& RecordHdu::as() const {
  return dynamic_cast<const T&>(*this);
}

template <typename T>
Record<T> RecordHdu::parseRecord(const std::string& keyword) const {
  return m_header.parse<T>(keyword);
}

template <typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parseRecords(const std::vector<std::string>& keywords) const {
  touchThisHdu();
  return Cfitsio::Header::parseRecords<Ts...>(m_fptr, keywords);
}

template <typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parseRecords(const Named<Ts>&... keywords) const {
  return m_header.parseSeq(keywords...);
}

template <class TReturn, typename... Ts>
TReturn RecordHdu::parseRecordsAs(const std::vector<std::string>& keywords) const {
  touchThisHdu();
  return Cfitsio::Header::parseRecordsAs<TReturn, Ts...>(m_fptr, keywords);
}

template <class TReturn, typename... Ts>
TReturn RecordHdu::parseRecordsAs(const Named<Ts>&... keywords) const {
  return m_header.parseStruct<TReturn>(keywords...);
}

template <typename T>
Record<T> RecordHdu::parseRecordOr(const Record<T>& fallback) const {
  return m_header.parseOr(fallback);
}

template <typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parseRecordsOr(const Record<Ts>&... fallbacks) const {
  return m_header.parseSeqOr(fallbacks...);
}

template <typename T>
RecordVector<T> RecordHdu::parseRecordVector(const std::vector<std::string>& keywords) const {
  return m_header.parseSeq<T>(keywords);
}

template <typename T>
RecordVector<T> RecordHdu::parseAllRecords() const {
  return m_header.parseSeq<T>(readKeywords());
}

template <typename T>
void RecordHdu::writeRecord(const Record<T>& record) const {
  m_header.write<RecordMode::CreateNew>(record);
}

template <typename T>
void RecordHdu::writeRecord(const std::string& k, T v, const std::string& u, const std::string& c) const {
  m_header.write<RecordMode::CreateNew>(k, v, u, c);
}

template <typename... Ts>
void RecordHdu::writeRecords(const Record<Ts>&... records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records...);
}

template <typename... Ts>
void RecordHdu::writeRecords(const std::tuple<Record<Ts>...>& records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records);
}

template <typename T>
void RecordHdu::writeRecords(const std::vector<Record<T>>& records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records);
}

template <typename T>
void RecordHdu::writeRecords(const RecordVector<T>& records, const std::vector<std::string>& keywords) const {
  m_header.writeSeqIn<RecordMode::CreateNew>(keywords, records.vector);
}

template <typename T>
void RecordHdu::updateRecord(const Record<T>& record) const {
  m_header.write<RecordMode::CreateOrUpdate>(record);
}

template <typename T>
void RecordHdu::updateRecord(const std::string& k, T v, const std::string& u, const std::string& c) const {
  m_header.write<RecordMode::CreateOrUpdate>(k, v, c, u);
}

template <typename... Ts>
void RecordHdu::updateRecords(const Record<Ts>&... records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records...);
}

template <typename... Ts>
void RecordHdu::updateRecords(const std::tuple<Record<Ts>...>& records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records);
}

template <typename T>
void RecordHdu::updateRecords(const std::vector<Record<T>>& records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records);
}

template <typename T>
void RecordHdu::updateRecords(const RecordVector<T>& records, const std::vector<std::string>& keywords) const {
  m_header.writeSeqIn<RecordMode::CreateOrUpdate>(keywords, records.vector);
}

  #ifndef DECLARE_PARSE_RECORD
    #define DECLARE_PARSE_RECORD(type, unused) \
      extern template Record<type> RecordHdu::parseRecord(const std::string&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(DECLARE_PARSE_RECORD)
    #undef DECLARE_PARSE_RECORD
  #endif

extern template RecordVector<VariantValue> RecordHdu::parseRecordVector(const std::vector<std::string>&) const;

  #ifndef DECLARE_WRITE_RECORD
    #define DECLARE_WRITE_RECORD(type, unused) extern template void RecordHdu::writeRecord(const Record<type>&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(DECLARE_WRITE_RECORD)
    #undef DECLARE_WRITE_RECORD
  #endif

extern template void RecordHdu::writeRecords(const std::vector<Record<VariantValue>>&) const;

} // namespace FitsIO
} // namespace Euclid

#endif
