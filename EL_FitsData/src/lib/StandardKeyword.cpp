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

#include "EL_FitsData/StandardKeyword.h"

#include <algorithm> // copy_if, find_if

namespace Euclid {
namespace FitsIO {

const std::vector<std::string> StandardKeyword::m_mandatories = { "SIMPLE",   "BITPIX", "NAXIS",  "NAXISn", "END",
                                                                  "XTENSION", "PCOUNT", "GCOUNT", "EXTEND" };

const std::vector<std::string> StandardKeyword::m_reserveds = {
  "AUTHOR",  "BLANK",    "BLOCKED", "BSCALE",   "BUNIT",  "BZERO",    "CDELTn",  "CROTAn",  "CRPIXn",
  "CRVALn",  "CTYPEn",   "DATAMAX", "DATAMIN",  "DATE",   "DATE-OBS", "EPOCH",   "EQUINOX", "EXTLEVEL",
  "EXTNAME", "EXTVER",   "GROUPS",  "INSTRUME", "OBJECT", "OBSERVER", "ORIGIN",  "PSCALn",  "PTYPEn",
  "PZEROn",  "REFERENC", "TBCOLn",  "TDIMn",    "TDISPn", "TELESCOP", "TFIELDS", "TFORMn",  "THEAP",
  "TNULLn",  "TSCALn",   "TTYPEn",  "TUNITn",   "TZEROn"
};

const std::vector<std::string> StandardKeyword::m_comments = { "COMMENT", "HISTORY" };

const std::map<KeywordCategory, const std::vector<std::string> &> StandardKeyword::byCategory() {
  return { { KeywordCategory::Mandatory, m_mandatories },
           { KeywordCategory::Reserved, m_reserveds },
           { KeywordCategory::Comment, m_comments } };
}

std::vector<std::string>
StandardKeyword::filterCategories(const std::vector<std::string> &keywords, KeywordCategory categories) {
  std::vector<std::string> res;
  const auto it = std::copy_if(keywords.begin(), keywords.end(), res.begin(), [&](const std::string &k) {
    return belongsCategories(k, categories);
  });
  res.resize(std::distance(res.begin(), it));
  return res;
}

bool StandardKeyword::belongsCategories(const std::string &keyword, KeywordCategory categories) {
  const auto standards = byCategory();
  for (const auto &s : standards) {
    if (categories & s.first) {
      if (matchesOneOf(keyword, s.second)) {
        return true;
      }
    }
  }
  // At that point, we know the keyword is not in the selected standard categories.
  if (categories & KeywordCategory::User) {
    for (const auto &s : standards) {
      if (matchesOneOf(keyword, s.second)) { // TODO could smarter and not redo matching
        return false;
      }
    }
    return true;
  }
  // At this point, we know the keyword is not in the selected categories.
  return false;
}

bool StandardKeyword::matches(const std::string &test, const std::string &ref) {
  if (test == ref) {
    return true;
  }
  return matchesIndexed(test, ref);
}

bool StandardKeyword::matchesIndexed(const std::string &test, const std::string &ref) {
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

bool StandardKeyword::matchesOneOf(const std::string &keyword, const std::vector<std::string> &refs) {
  const auto pos = std::find_if(refs.begin(), refs.end(), [&](const std::string &test) {
    return matches(keyword, test);
  });
  return pos != refs.end();
}

} // namespace FitsIO
} // namespace Euclid
