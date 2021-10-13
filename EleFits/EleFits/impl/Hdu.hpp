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

#if defined(_ELEFITS_HDU_IMPL) || defined(CHECK_QUALITY)

  #include "EleCfitsioWrapper/HeaderWrapper.h"
  #include "EleFitsData/RecordVec.h"
  #include "EleFits/Hdu.h"

namespace Euclid {
namespace Fits {

template <typename T>
const T& Hdu::as() const {
  return dynamic_cast<const T&>(*this);
}

template <typename T>
Record<T> Hdu::parseRecord(const std::string& keyword) const {
  return m_header.parse<T>(keyword);
}

template <typename... Ts>
std::tuple<Record<Ts>...> Hdu::parseRecords(const std::vector<std::string>& keywords) const {
  touchThisHdu();
  return Cfitsio::HeaderIo::parseRecords<Ts...>(m_fptr, keywords);
}

template <typename... Ts>
std::tuple<Record<Ts>...> Hdu::parseRecords(const Named<Ts>&... keywords) const {
  return m_header.parseSeq(keywords...);
}

template <class TReturn, typename... Ts>
TReturn Hdu::parseRecordsAs(const std::vector<std::string>& keywords) const {
  touchThisHdu();
  return Cfitsio::HeaderIo::parseRecordsAs<TReturn, Ts...>(m_fptr, keywords);
}

template <class TReturn, typename... Ts>
TReturn Hdu::parseRecordsAs(const Named<Ts>&... keywords) const {
  return m_header.parseStruct<TReturn>(keywords...);
}

template <typename T>
Record<T> Hdu::parseRecordOr(const Record<T>& fallback) const {
  return m_header.parseOr(fallback);
}

template <typename... Ts>
std::tuple<Record<Ts>...> Hdu::parseRecordsOr(const Record<Ts>&... fallbacks) const {
  return m_header.parseSeqOr(fallbacks...);
}

template <typename T>
RecordVec<T> Hdu::parseRecordVector(const std::vector<std::string>& keywords) const {
  return m_header.parseSeq<T>(keywords);
}

template <typename T>
RecordVec<T> Hdu::parseAllRecords() const {
  return m_header.parseSeq<T>(readKeywords(~KeywordCategory::Comment));
}

template <typename T>
void Hdu::writeRecord(const Record<T>& record) const {
  m_header.write<RecordMode::CreateNew>(record);
}

template <typename T>
void Hdu::writeRecord(const std::string& k, T v, const std::string& u, const std::string& c) const {
  m_header.write<RecordMode::CreateNew>(k, v, u, c);
}

template <typename... Ts>
void Hdu::writeRecords(const Record<Ts>&... records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records...);
}

template <typename... Ts>
void Hdu::writeRecords(const std::tuple<Record<Ts>...>& records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records);
}

template <typename T>
void Hdu::writeRecords(const std::vector<Record<T>>& records) const {
  m_header.writeSeq<RecordMode::CreateNew>(records);
}

template <typename T>
void Hdu::writeRecords(const RecordVec<T>& records, const std::vector<std::string>& keywords) const {
  m_header.writeSeqIn<RecordMode::CreateNew>(keywords, records.vector);
}

template <typename T>
void Hdu::updateRecord(const Record<T>& record) const {
  m_header.write<RecordMode::CreateOrUpdate>(record);
}

template <typename T>
void Hdu::updateRecord(const std::string& k, T v, const std::string& u, const std::string& c) const {
  m_header.write<RecordMode::CreateOrUpdate>(k, v, u, c);
}

template <typename... Ts>
void Hdu::updateRecords(const Record<Ts>&... records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records...);
}

template <typename... Ts>
void Hdu::updateRecords(const std::tuple<Record<Ts>...>& records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records);
}

template <typename T>
void Hdu::updateRecords(const std::vector<Record<T>>& records) const {
  m_header.writeSeq<RecordMode::CreateOrUpdate>(records);
}

template <typename T>
void Hdu::updateRecords(const RecordVec<T>& records, const std::vector<std::string>& keywords) const {
  m_header.writeSeqIn<RecordMode::CreateOrUpdate>(keywords, records.vector);
}

  #ifndef DECLARE_PARSE_RECORD
    #define DECLARE_PARSE_RECORD(type, unused) extern template Record<type> Hdu::parseRecord(const std::string&) const;
ELEFITS_FOREACH_RECORD_TYPE(DECLARE_PARSE_RECORD)
    #undef DECLARE_PARSE_RECORD
  #endif

extern template RecordSeq Hdu::parseRecordVector(const std::vector<std::string>&) const;

  #ifndef DECLARE_WRITE_RECORD
    #define DECLARE_WRITE_RECORD(type, unused) extern template void Hdu::writeRecord(const Record<type>&) const;
ELEFITS_FOREACH_RECORD_TYPE(DECLARE_WRITE_RECORD)
    #undef DECLARE_WRITE_RECORD
  #endif

extern template void Hdu::writeRecords(const std::vector<Record<VariantValue>>&) const;

} // namespace Fits
} // namespace Euclid

#endif
