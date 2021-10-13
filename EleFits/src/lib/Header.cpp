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

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFits/Hdu.h"

#include <algorithm> // find

namespace Euclid {
namespace Fits {

Header::Header(fitsfile*& fptr, std::function<void(void)> touchFunction, std::function<void(void)> editFunction) :
    m_fptr(fptr), m_touch(touchFunction), m_edit(editFunction) {}

bool Header::has(const std::string& keyword) const {
  m_touch();
  return Cfitsio::HeaderIo::hasKeyword(m_fptr, keyword);
}

void Header::remove(const std::string& keyword) const {
  m_edit();
  KeywordNotFoundError::mayThrow(keyword, *this);
  Cfitsio::HeaderIo::deleteRecord(m_fptr, keyword);
}

std::vector<std::string> Header::readKeywords(KeywordCategory categories) const {
  m_touch();
  return Cfitsio::HeaderIo::listKeywords(m_fptr, categories);
}

std::map<std::string, std::string> Header::readKeywordsValues(KeywordCategory categories) const {
  m_touch();
  return Cfitsio::HeaderIo::listKeywordsValues(m_fptr, categories);
}

std::string Header::readAll(KeywordCategory categories) const {
  m_touch();
  const bool incNonValues = categories == KeywordCategory::All;
  return Cfitsio::HeaderIo::readHeader(m_fptr, incNonValues);
}

RecordSeq Header::parseAll(KeywordCategory categories) const {
  return parseSeq<VariantValue>(readKeywords(categories & ~KeywordCategory::Comment));
  // TODO return comments as string Records?
}

KeywordExistsError::KeywordExistsError(const std::string& existingKeyword) :
    FitsError(std::string("Keyword already exists: ") + existingKeyword), keyword(existingKeyword) {}

void KeywordExistsError::mayThrow(const std::string& existingKeyword, const Header& header) {
  if (header.has(existingKeyword)) {
    throw KeywordExistsError(existingKeyword);
  }
}

void KeywordExistsError::mayThrow(const std::vector<std::string>& existingKeywords, const Header& header) {
  const auto found = header.readKeywords();
  for (const auto& k : existingKeywords) {
    if (std::find(found.begin(), found.end(), k) != found.end()) {
      throw KeywordExistsError(k);
    }
  }
}

KeywordNotFoundError::KeywordNotFoundError(const std::string& missingKeyword) :
    FitsError(std::string("Keyword not found: ") + missingKeyword), keyword(missingKeyword) {}

void KeywordNotFoundError::mayThrow(const std::string& missingKeyword, const Header& header) {
  if (not header.has(missingKeyword)) {
    throw KeywordNotFoundError(missingKeyword);
  }
}

void KeywordNotFoundError::mayThrow(const std::vector<std::string>& missingKeywords, const Header& header) {
  const auto found = header.readKeywords();
  for (const auto& k : missingKeywords) {
    if (std::find(found.begin(), found.end(), k) == found.end()) {
      throw KeywordNotFoundError(k);
    }
  }
}

} // namespace Fits
} // namespace Euclid