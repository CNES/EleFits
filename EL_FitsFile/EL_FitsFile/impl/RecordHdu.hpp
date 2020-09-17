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

#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
Record<T> RecordHdu::parseRecord(const std::string &keyword) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecord<T>(m_fptr, keyword);
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

#ifndef DECLARE_PARSE_RECORD
#define DECLARE_PARSE_RECORD(T) extern template Record<T> RecordHdu::parseRecord(const std::string &) const;
DECLARE_PARSE_RECORD(char)
DECLARE_PARSE_RECORD(short)
DECLARE_PARSE_RECORD(int)
DECLARE_PARSE_RECORD(long)
DECLARE_PARSE_RECORD(float)
DECLARE_PARSE_RECORD(double)
DECLARE_PARSE_RECORD(unsigned char)
DECLARE_PARSE_RECORD(unsigned short)
DECLARE_PARSE_RECORD(unsigned int)
DECLARE_PARSE_RECORD(unsigned long)
#undef DECLARE_PARSE_RECORD
#endif

extern template RecordVector<boost::any> RecordHdu::parseRecordVector(const std::vector<std::string> &) const;

#ifndef DECLARE_WRITE_RECORD
#define DECLARE_WRITE_RECORD(T) \
  extern template void RecordHdu::writeRecord(const Record<T> &) const; \
  extern template void RecordHdu::updateRecord(const Record<T> &) const;
DECLARE_WRITE_RECORD(char)
DECLARE_WRITE_RECORD(short)
DECLARE_WRITE_RECORD(int)
DECLARE_WRITE_RECORD(long)
DECLARE_WRITE_RECORD(float)
DECLARE_WRITE_RECORD(double)
DECLARE_WRITE_RECORD(unsigned char)
DECLARE_WRITE_RECORD(unsigned short)
DECLARE_WRITE_RECORD(unsigned int)
DECLARE_WRITE_RECORD(unsigned long)
#undef DECLARE_WRITE_RECORD
#endif

extern template void RecordHdu::writeRecords(const std::vector<Record<boost::any>> &) const;
extern template void RecordHdu::updateRecords(const std::vector<Record<boost::any>> &) const;

} // namespace FitsIO
} // namespace Euclid
