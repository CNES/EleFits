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

#if defined(_EL_FITSFILE_HEADER_IMPL) || defined(CHECK_QUALITY)

  #include "EL_CfitsioWrapper/HeaderWrapper.h"
  #include "EL_FitsFile/Header.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
Record<T> Header::parse(const std::string& keyword) const {
  m_touch();
  return Cfitsio::Header::parseRecord<T>(m_fptr, keyword);
}

template <typename T>
Record<T> Header::parseOr(const Record<T>& fallback) const {
  if (has(fallback.keyword)) {
    return parse<T>(fallback.keyword);
  }
  return fallback;
}

template <typename T>
Record<T> Header::parseOr(
    const std::string& keyword,
    const T& fallbackValue,
    const std::string& fallbackUnit,
    const std::string& fallbackComment) const {
  return parseOr(Record<T>(keyword, fallbackValue, fallbackUnit, fallbackComment));
}

template <typename... Ts>
std::tuple<Record<Ts>...> Header::parseTuple(const Named<Ts>&... keywords) const {
  return parseStruct<std::tuple<Record<Ts>...>, Ts...>(keywords...);
}

template <typename TReturn, typename... Ts>
TReturn Header::parseStruct(const Named<Ts>&... keywords) const {
  m_touch();
  return { Cfitsio::Header::parseRecord<Ts>(m_fptr, keywords.name)... };
}

template <typename... Ts>
std::tuple<Record<Ts>...> Header::parseTupleOr(const std::tuple<Record<Ts>...>& fallbacks) const {
  auto func = [&](const auto&... fs) {
    return this->parseTupleOr(fs...);
  };
  return tupleApply(fallbacks, func);
}

template <typename... Ts>
std::tuple<Record<Ts>...> Header::parseTupleOr(const Record<Ts>&... fallbacks) const {
  std::tuple<Record<Ts>...> t { parseOr<Ts>(fallbacks)... }; // TODO avoid calling touchThisHdu for each keyword
  return std::tuple<Record<Ts>...> { t };
}

template <typename T>
RecordVector<T> Header::parseVector(const std::vector<std::string>& keywords) const {
  m_touch();
  RecordVector<T> res(keywords.size());
  std::transform(keywords.begin(), keywords.end(), res.vector.begin(), [&](const std::string& k) {
    return Cfitsio::Header::parseRecord<T>(m_fptr, k);
  });
  return res;
}

template <typename T>
RecordVector<T> Header::parseVectorOr(const std::vector<Record<T>>& fallbacks) const {
  m_touch();
  RecordVector<T> v(0);
  for (const auto& f : fallbacks) {
    v.push_back(parseOr(f)); // TODO avoid calling touchThisHdu for each keyword
  }
  return v;
}

/// @cond INTERNAL
namespace Internal {

template <RecordMode Mode>
struct RecordWriterImpl {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record);
};

template <>
struct RecordWriterImpl<RecordMode::CreateUnique> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record) {
    KeywordExistsError::mayThrow(record.keyword, header);
    Cfitsio::Header::writeRecord(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::CreateNew> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record) {
    Cfitsio::Header::writeRecord(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::UpdateExisting> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record) {
    KeywordNotFoundError::mayThrow(record.keyword, header);
    Cfitsio::Header::updateRecord(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::CreateOrUpdate> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record) {
    Cfitsio::Header::updateRecord(fptr, record);
  }
};

} // namespace Internal

template <RecordMode Mode, typename T>
void Header::write(const Record<T>& record) const {
  m_edit();
  Internal::RecordWriterImpl<Mode>::write(m_fptr, *this, record);
}

template <RecordMode Mode, typename T>
void Header::write(const std::string& keyword, const T& value, const std::string& unit, const std::string& comment)
    const {
  return write<Mode, T>({ keyword, value, unit, comment });
}

template <RecordMode Mode, typename... Ts>
void Header::writeTuple(const std::tuple<Record<Ts>...>& records) const {
  m_edit();
  auto func = [&](const auto& r) {
    Internal::RecordWriterImpl<Mode>::write(m_fptr, *this, r);
  };
  tupleForeach(records, func);
}

template <RecordMode Mode, typename... Ts>
void Header::writeTupleIn(const std::vector<std::string>& keywords, const std::tuple<Record<Ts>...>& records) const {
  // FIXME
}

template <RecordMode Mode, typename... Ts>
void Header::writeTuple(const Record<Ts>&... records) const {
  m_edit();
  Cfitsio::Header::writeRecords(m_fptr, records...); // FIXME Mode
}

template <RecordMode Mode, typename... Ts>
void Header::writeTupleIn(const std::vector<std::string>& keywords, const Record<Ts>&... records) const {
  m_edit();
  // FIXME
}

template <RecordMode mode, typename T>
void Header::writeVector(const std::vector<Record<T>>& records) const {
  m_edit();
  for (const auto& r : records) {
    Cfitsio::Header::writeRecord(m_fptr, r);
  }
}

template <RecordMode mode, typename T>
void Header::writeVectorIn(const std::vector<std::string>& keywords, const std::vector<Record<T>>& records) const {
  m_edit();
  RecordVector<T> v(records);
  for (const auto& k : keywords) {
    Cfitsio::Header::writeRecord(m_fptr, v[k]);
  }
}

} // namespace FitsIO
} // namespace Euclid

#endif