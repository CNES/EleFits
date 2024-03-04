// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_HEADER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFits/Header.h"
#include "Linx/Base/SeqUtils.h" // seq_foreach

namespace Euclid {
namespace Fits {

template <typename T>
Record<T> Header::parse(const std::string& keyword) const
{
  m_touch();
  return Cfitsio::HeaderIo::parse_record<T>(m_fptr, keyword);
}

template <typename T>
Record<T> Header::parse_or(const Record<T>& fallback) const
{
  if (has(fallback.keyword)) {
    return parse<T>(fallback.keyword);
  }
  return fallback;
}

template <typename T>
Record<T> Header::parse_or(
    const std::string& keyword,
    T fallback_value,
    const std::string& fallback_unit,
    const std::string& fallback_comment) const
{
  return parse_or<T>({keyword, fallback_value, fallback_unit, fallback_comment});
}

template <typename T>
RecordVec<T> Header::parse_n(const std::vector<std::string>& keywords) const
{
  m_touch();
  RecordVec<T> res(keywords.size());
  std::transform(keywords.begin(), keywords.end(), res.vector.begin(), [&](const std::string& k) {
    return Cfitsio::HeaderIo::parse_record<T>(m_fptr, k);
  });
  return res;
}

template <typename... Ts>
std::tuple<Record<Ts>...> Header::parse_n(const TypedKey<Ts, std::string>&... keywords) const
{
  return parse_struct<std::tuple<Record<Ts>...>, Ts...>(keywords...);
}

template <typename TSeq>
TSeq Header::parse_n_or(TSeq&& fallbacks) const
{
  auto func = [&](const auto& f) {
    return parse_or(f);
  };
  return seq_transform<TSeq>(fallbacks, func);
}

template <typename... Ts>
std::tuple<Record<Ts>...> Header::parse_n_or(const Record<Ts>&... fallbacks) const
{
  return parse_struct_or<std::tuple<Record<Ts>...>, Ts...>(fallbacks...);
}

template <typename TReturn, typename... Ts>
TReturn Header::parse_struct(const TypedKey<Ts, std::string>&... keywords) const
{
  m_touch();
  return {Cfitsio::HeaderIo::parse_record<Ts>(m_fptr, keywords.key)...};
}

template <typename TReturn, typename... Ts>
TReturn Header::parse_struct_or(const Record<Ts>&... fallbacks) const
{
  return {parse_or<Ts>(fallbacks)...}; // TODO avoid calling touch for each keyword
}

template <typename TReturn, typename TSeq>
TReturn Header::parse_struct_or(TSeq&& fallbacks) const
{
  return seq_transform<TReturn>(fallbacks, [&](auto f) {
    return parse_or(f);
  }); // FIXME test
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
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record)
  {
    KeywordExistsError::may_throw(record.keyword, header);
    Cfitsio::HeaderIo::write_record(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::CreateNew> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record)
  {
    (void)(header);
    Cfitsio::HeaderIo::write_record(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::UpdateExisting> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record)
  {
    KeywordNotFoundError::may_throw(record.keyword, header);
    Cfitsio::HeaderIo::update_record(fptr, record);
  }
};

template <>
struct RecordWriterImpl<RecordMode::CreateOrUpdate> {
  template <typename T>
  static void write(fitsfile* fptr, const Header& header, const Record<T>& record)
  {
    (void)(header);
    Cfitsio::HeaderIo::update_record(fptr, record);
  }
};

} // namespace Internal

template <RecordMode Mode, typename T>
void Header::write(const Record<T>& record) const
{
  m_edit();
  Internal::RecordWriterImpl<Mode>::write(m_fptr, *this, record);
}

template <RecordMode Mode, typename T>
void Header::write(const std::string& keyword, T value, const std::string& unit, const std::string& comment) const
{
  write<Mode, T>({keyword, value, unit, comment});
}

template <>
void Header::write<RecordMode::CreateOrUpdate, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const; // TODO dispatch Mode

template <>
void Header::write<RecordMode::CreateUnique, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const; // TODO dispatch Mode

template <>
void Header::write<RecordMode::CreateNew, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const; // TODO dispatch Mode

template <>
void Header::write<RecordMode::UpdateExisting, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const; // TODO dispatch Mode

template <RecordMode Mode, typename... Ts>
void Header::write_n(const Record<Ts>&... records) const
{
  write_n<Mode>(std::forward_as_tuple(records...));
}

template <RecordMode Mode, typename TSeq>
void Header::write_n(TSeq&& records) const
{
  m_edit();
  auto func = [&](const auto& r) {
    Internal::RecordWriterImpl<Mode>::write(m_fptr, *this, r);
  };
  Linx::seq_foreach(LINX_FORWARD(records), func);
}

template <RecordMode Mode, typename... Ts>
void Header::write_n_in(const std::vector<std::string>& keywords, const Record<Ts>&... records) const
{
  write_n_in<Mode>(keywords, std::forward_as_tuple(records...));
}

template <RecordMode Mode, typename TSeq>
void Header::write_n_in(const std::vector<std::string>& keywords, TSeq&& records) const
{
  m_edit();
  auto func = [&](const auto& r) {
    if (std::find(keywords.begin(), keywords.end(), r.keyword) != keywords.end()) {
      Internal::RecordWriterImpl<Mode>::write(m_fptr, *this, r);
    }
  };
  seq_foreach(std::forward<TSeq>(records), func);
}

#ifndef DECLARE_PARSE
#define DECLARE_PARSE(type, unused) extern template Record<type> Header::parse(const std::string&) const;
ELEFITS_FOREACH_RECORD_TYPE(DECLARE_PARSE)
#undef DECLARE_PARSE
#endif

#ifndef DECLARE_WRITE_RECORD
#define DECLARE_WRITE_RECORD(type, unused) extern template void Header::write(const Record<type>&) const;
ELEFITS_FOREACH_RECORD_TYPE(DECLARE_WRITE_RECORD)
#undef DECLARE_WRITE_RECORD
#endif

// TODO extern declare more?

} // namespace Fits
} // namespace Euclid

#endif
