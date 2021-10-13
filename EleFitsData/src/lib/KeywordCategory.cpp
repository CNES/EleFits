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

#include "EleFitsData/KeywordCategory.h"

#include <algorithm> // copy_if, find_if

namespace Euclid {
namespace Fits {

const KeywordCategory KeywordCategory::Mandatory { 0b0001 };
const KeywordCategory KeywordCategory::Reserved { 0b0010 };
const KeywordCategory KeywordCategory::Comment { 0b0100 };
const KeywordCategory KeywordCategory::User { 0b1000 };
const KeywordCategory KeywordCategory::None { 0b0000 };
const KeywordCategory KeywordCategory::All { ~None };

const std::vector<std::string> KeywordCategory::m_mandatories = { "SIMPLE",   "BITPIX", "NAXIS",  "NAXISn", "END",
                                                                  "XTENSION", "PCOUNT", "GCOUNT", "EXTEND" };

const std::vector<std::string> KeywordCategory::m_reserveds = {
  "AUTHOR",  "BLANK",    "BLOCKED", "BSCALE",   "BUNIT",  "BZERO",    "CDELTn",  "CROTAn",  "CRPIXn",
  "CRVALn",  "CTYPEn",   "DATAMAX", "DATAMIN",  "DATE",   "DATE-OBS", "EPOCH",   "EQUINOX", "EXTLEVEL",
  "EXTNAME", "EXTVER",   "GROUPS",  "INSTRUME", "OBJECT", "OBSERVER", "ORIGIN",  "PSCALn",  "PTYPEn",
  "PZEROn",  "REFERENC", "TBCOLn",  "TDIMn",    "TDISPn", "TELESCOP", "TFIELDS", "TFORMn",  "THEAP",
  "TNULLn",  "TSCALn",   "TTYPEn",  "TUNITn",   "TZEROn"
};

const std::vector<std::string> KeywordCategory::m_comments = { "COMMENT", "HISTORY" };

const std::map<int, const std::vector<std::string>&> KeywordCategory::byCategory() {
  return { { Mandatory.m_category, m_mandatories },
           { Reserved.m_category, m_reserveds },
           { Comment.m_category, m_comments } };
}

KeywordCategory::KeywordCategory(int category) : m_category(category) {}

std::vector<std::string>
KeywordCategory::filterCategories(const std::vector<std::string>& keywords, KeywordCategory categories) {
  std::vector<std::string> res;
  const auto it = std::copy_if(keywords.begin(), keywords.end(), res.begin(), [&](const std::string& k) {
    return belongsCategories(k, categories);
  });
  res.resize(std::distance(res.begin(), it));
  return res;
}

bool KeywordCategory::belongsCategories(const std::string& keyword, KeywordCategory categories) {
  const auto standards = byCategory();
  for (const auto& s : standards) { // TODO Could be std::any_of but would it be readable?
    if (categories & KeywordCategory(s.first)) {
      if (matchesOneOf(keyword, s.second)) {
        return true;
      }
    }
  }
  // At that point, we know the keyword is not in the selected standard categories.
  if (categories & User) {
    for (const auto& s : standards) { // TODO Could be std::none_of but would it be readable?
      if (matchesOneOf(keyword, s.second)) {
        // TODO could smarter and not redo matching (e.g. store category vs. match)
        return false;
      }
    }
    return true;
  }
  // At this point, we know the keyword is not in the selected categories.
  return false;
}

bool KeywordCategory::matches(const std::string& test, const std::string& ref) {
  if (test == ref) {
    return true;
  }
  return matchesIndexed(test, ref);
}

bool KeywordCategory::matchesIndexed(const std::string& test, const std::string& ref) {
  const auto split = ref.length() - 1;
  if (test.length() <= split) {
    return false;
  }
  const auto name = test.substr(0, split);
  const auto index = test.substr(split);
  if (name != ref.substr(0, split)) {
    return false;
  }
  if (index.length() == 0) {
    return false;
  }
  for (const auto c : index) {
    if (not std::isdigit(c)) {
      return false;
    }
  }
  return true;
}

bool KeywordCategory::matchesOneOf(const std::string& keyword, const std::vector<std::string>& refs) {
  const auto pos = std::find_if(refs.begin(), refs.end(), [&](const std::string& test) {
    return matches(keyword, test);
  });
  return pos != refs.end();
}

} // namespace Fits
} // namespace Euclid
