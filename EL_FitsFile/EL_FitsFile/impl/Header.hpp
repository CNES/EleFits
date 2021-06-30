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
  #include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
Record<std::decay_t<T>> Header::parse1(const std::string& keyword) const {
  m_hdu.touchThisHdu();
  return Cfitsio::Header::parseRecord<T>(m_hdu.m_fptr, keyword);
}

template <typename T>
Record<std::decay_t<T>> Header::parse1Or(const Record<T>& fallback) const {
  if (has(fallback.keyword)) {
    return parse1<T>(fallback.keyword);
  }
  return fallback;
}

template <typename T>
Record<std::decay_t<T>> Header::parse1Or(
    const std::string& keyword,
    const T& fallbackValue,
    const std::string& fallbackUnit,
    const std::string& fallbackComment) const {
  return parse1Or(Record<T>(keyword, fallbackValue, fallbackUnit, fallbackComment));
}

template <typename... Ts>
RecordTuple<std::decay_t<Ts>...> Header::parseN(const std::tuple<Named<Ts>...>& keywords) const {
  return RecordTuple<std::decay_t<Ts>...>(); // FIXME
}

template <typename... Ts>
RecordTuple<std::decay_t<Ts>...> Header::parseN(const Named<Ts>&... keywords) const {
  return parseN<Ts...>({ keywords.name... });
}

template <typename... Ts>
RecordTuple<std::decay_t<Ts>...> Header::parseNOr(const std::tuple<Record<Ts>...>& fallbacks) const {
  // FIXME
  return { fallbacks };
}

template <typename... Ts>
RecordTuple<std::decay_t<Ts>...> Header::parseNOr(const Record<Ts>&... fallbacks) const {
  return { { parse1Or<Ts>(fallbacks)... } }; // TODO avoid calling touchThisHdu for each keyword
}

template <typename T>
RecordVector<std::decay_t<T>> Header::parseN(const std::vector<std::string>& keywords) const {
  RecordVector<std::decay_t<T>> res(keywords.size());
  std::transform(keywords.begin(), keywords.end(), res.vector.begin(), [&](const std::string& k) {
    return parse1<T>(k);
  });
  return res;
}

template <typename T>
RecordVector<std::decay_t<T>> Header::parseNOr(const std::vector<Record<T>>& fallbacks) const {
  // FIXME
  return fallbacks;
}

/// @cond INTERNAL
namespace Internal {

template <RecordMode Mode, typename T>
struct RecordWriterImpl {
  static void write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record);
};

template <typename T>
struct RecordWriterImpl<RecordMode::CreateUnique, T> {
  static void write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record);
};

template <typename T>
struct RecordWriterImpl<RecordMode::CreateNew, T> {
  static void write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record);
};

template <typename T>
struct RecordWriterImpl<RecordMode::UpdateExisting, T> {
  static void write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record);
};

template <typename T>
struct RecordWriterImpl<RecordMode::CreateOrUpdate, T> {
  static void write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record);
};

template <typename T>
void RecordWriterImpl<RecordMode::CreateUnique, T>::write1(
    fitsfile* fptr,
    const RecordHdu& hdu,
    const Record<T>& record) {
  KeywordExistsError::mayThrow(record.keyword, hdu);
  Cfitsio::Header::writeRecord(fptr, record);
}

template <typename T>
void RecordWriterImpl<RecordMode::CreateNew, T>::write1(fitsfile* fptr, const RecordHdu& hdu, const Record<T>& record) {
  Cfitsio::Header::writeRecord(fptr, record);
}

template <typename T>
void RecordWriterImpl<RecordMode::UpdateExisting, T>::write1(
    fitsfile* fptr,
    const RecordHdu& hdu,
    const Record<T>& record) {
  KeywordNotFoundError::mayThrow(record.keyword, hdu);
  Cfitsio::Header::updateRecord(fptr, record);
}

template <typename T>
void RecordWriterImpl<RecordMode::CreateOrUpdate, T>::write1(
    fitsfile* fptr,
    const RecordHdu& hdu,
    const Record<T>& record) {
  Cfitsio::Header::updateRecord(fptr, record);
}

} // namespace Internal

template <RecordMode Mode, typename T>
void Header::write1(const Record<T>& record) const {
  m_hdu.editThisHdu();
  Internal::RecordWriterImpl<Mode, T>::write1(m_hdu.m_fptr, m_hdu, record);
}

template <RecordMode Mode, typename T>
void Header::write1(const std::string& keyword, const T& value, const std::string& unit, const std::string& comment)
    const {
  return write1<Mode, T>({ keyword, value, unit, comment });
}

template <RecordMode Mode, typename... Ts>
void Header::writeN(const std::tuple<Record<Ts>...>& records) const {
  m_hdu.editThisHdu();
  Cfitsio::Header::writeRecords(m_hdu.m_fptr, records); // FIXME Mode
}

template <RecordMode Mode, typename... Ts>
void Header::writeN(const std::vector<std::string>& keywords, const std::tuple<Record<Ts>...>& records) const {
  // FIXME
}

template <RecordMode Mode, typename... Ts>
void Header::writeN(const Record<Ts>&... records) const {
  m_hdu.editThisHdu();
  Cfitsio::Header::writeRecords(m_hdu.m_fptr, records...); // FIXME Mode
}

template <RecordMode Mode, typename... Ts>
void Header::writeN(const std::vector<std::string>& keywords, const Record<Ts>&... records) const {
  // FIXME
}

template <RecordMode mode, typename T>
void Header::writeN(const std::vector<Record<T>>& records) const {
  // FIXME
}

template <RecordMode mode, typename T>
void Header::writeN(const std::vector<std::string>& keywords, const std::vector<Record<T>>& records) const {
  // FIXME
}

} // namespace FitsIO
} // namespace Euclid

#endif