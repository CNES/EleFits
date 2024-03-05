// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFits/Hdu.h"

#include <algorithm> // find

namespace Euclid {
namespace Fits {

Header::Header(fitsfile*& fptr, std::function<void(void)> touch, std::function<void(void)> edit) :
    m_fptr(fptr), m_touch(touch), m_edit(edit)
{}

bool Header::has(const std::string& keyword) const
{
  m_touch();
  return Cfitsio::HeaderIo::has_keyword(m_fptr, keyword);
}

void Header::remove(const std::string& keyword) const
{
  m_edit();
  KeywordNotFoundError::may_throw(keyword, *this);
  Cfitsio::HeaderIo::remove_record(m_fptr, keyword);
}

std::vector<std::string> Header::read_all_keywords(KeywordCategory categories) const
{
  m_touch();
  return Cfitsio::HeaderIo::list_keywords(m_fptr, categories);
}

std::map<std::string, std::string> Header::read_all_keywords_values(KeywordCategory categories) const
{
  m_touch();
  return Cfitsio::HeaderIo::list_keywords_values(m_fptr, categories);
}

std::string Header::read_all(KeywordCategory categories) const
{
  m_touch();
  const bool inv_non_valued = categories == KeywordCategory::All;
  return Cfitsio::HeaderIo::read_header(m_fptr, inv_non_valued);
}

RecordSeq Header::parse_all(KeywordCategory categories) const
{
  return parse_n<VariantValue>(read_all_keywords(categories & ~KeywordCategory::Comment));
  // TODO return comments as string Records?
}

template <>
void Header::write<RecordMode::CreateOrUpdate, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const
{
  write<RecordMode::CreateOrUpdate>(keyword, std::string(value), unit, comment);
}

template <>
void Header::write<RecordMode::CreateUnique, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const
{
  write<RecordMode::CreateUnique>(keyword, std::string(value), unit, comment);
}

template <>
void Header::write<RecordMode::CreateNew, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const
{
  write<RecordMode::CreateNew>(keyword, std::string(value), unit, comment);
}

template <>
void Header::write<RecordMode::UpdateExisting, const char*>(
    const std::string& keyword,
    const char* value,
    const std::string& unit,
    const std::string& comment) const
{
  write<RecordMode::UpdateExisting>(keyword, std::string(value), unit, comment);
}

void Header::write_comment(const std::string& comment) const
{
  m_edit();
  return Cfitsio::HeaderIo::write_comment(m_fptr, comment);
}

void Header::write_history(const std::string& history) const
{
  m_edit();
  return Cfitsio::HeaderIo::write_history(m_fptr, history);
}

KeywordExistsError::KeywordExistsError(const std::string& existing_keyword) :
    FitsError(std::string("Keyword already exists: ") + existing_keyword), keyword(existing_keyword)
{}

void KeywordExistsError::may_throw(const std::string& existing_keyword, const Header& header)
{
  if (header.has(existing_keyword)) {
    throw KeywordExistsError(existing_keyword);
  }
}

void KeywordExistsError::may_throw(const std::vector<std::string>& existing_keywords, const Header& header)
{
  const auto found = header.read_all_keywords();
  for (const auto& k : existing_keywords) {
    if (std::find(found.begin(), found.end(), k) != found.end()) {
      throw KeywordExistsError(k);
    }
  }
}

KeywordNotFoundError::KeywordNotFoundError(const std::string& missing_keyword) :
    FitsError(std::string("Keyword not found: ") + missing_keyword), keyword(missing_keyword)
{}

void KeywordNotFoundError::may_throw(const std::string& missing_keyword, const Header& header)
{
  if (not header.has(missing_keyword)) {
    throw KeywordNotFoundError(missing_keyword);
  }
}

void KeywordNotFoundError::may_throw(const std::vector<std::string>& missing_keywords, const Header& header)
{
  const auto found = header.read_all_keywords();
  for (const auto& k : missing_keywords) {
    if (std::find(found.begin(), found.end(), k) == found.end()) {
      throw KeywordNotFoundError(k);
    }
  }
}

} // namespace Fits
} // namespace Euclid
