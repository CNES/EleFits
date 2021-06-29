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

Header::Header(const RecordHdu& hdu) : m_hdu(hdu) {}

KeywordExistsError::KeywordExistsError(const std::string& keyword) :
    FitsIOError(std::string("Keyword already exists: ") + keyword) {}

void KeywordExistsError::mayThrow(const std::string& keyword, const RecordHdu& hdu) {
  if (hdu.hasKeyword(keyword)) {
    throw KeywordExistsError(keyword);
  }
}

void KeywordExistsError::mayThrow(const std::vector<std::string>& keywords, const RecordHdu& hdu) {
  const auto found = hdu.readKeywords();
  for (const auto& k : keywords) {
    if (std::find(found.begin(), found.end(), k) != found.end()) {
      throw KeywordExistsError(k);
    }
  }
}

KeywordNotFoundError::KeywordNotFoundError(const std::string& keyword) :
    FitsIOError(std::string("Keyword not found: ") + keyword) {}

void KeywordNotFoundError::mayThrow(const std::string& keyword, const RecordHdu& hdu) {
  if (not hdu.hasKeyword(keyword)) {
    throw KeywordNotFoundError(keyword);
  }
}

void KeywordNotFoundError::mayThrow(const std::vector<std::string>& keywords, const RecordHdu& hdu) {
  const auto found = hdu.readKeywords();
  for (const auto& k : keywords) {
    if (std::find(found.begin(), found.end(), k) == found.end()) {
      throw KeywordNotFoundError(k);
    }
  }
}

bool Header::has(const std::string& keyword) const {
  m_hdu.touchThisHdu();
  return Cfitsio::Header::hasKeyword(m_hdu.m_fptr, keyword);
}

void Header::remove(const std::string& keyword) const {
  m_hdu.editThisHdu();
  KeywordNotFoundError::mayThrow(keyword, m_hdu);
  Cfitsio::Header::deleteRecord(m_hdu.m_fptr, keyword);
}

std::vector<std::string> Header::readKeywords(KeywordCategory categories) const {
  return {}; // FIXME
}

std::map<std::string, std::string> Header::readKeywordsValues(KeywordCategory categories) const {
  return {}; // FIXME
}

std::string Header::readAll(KeywordCategory categories) const {
  m_hdu.touchThisHdu();
  const bool comments = StandardKeyword::belongsCategories("COMMENT", categories); // TODO clean
  return Cfitsio::Header::readHeader(m_hdu.m_fptr, comments);
}

RecordVector<VariantValue> Header::parseAll(KeywordCategory categories) const {
  return parseN<VariantValue>(readKeywords(categories));
}

} // namespace FitsIO
} // namespace Euclid