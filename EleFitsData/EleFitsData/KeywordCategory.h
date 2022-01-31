// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_KEYWORDCATEGORY_H
#define _ELECFITSIOWRAPPER_KEYWORDCATEGORY_H

#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @ingroup iterators
 * @brief Keyword categories and related tools.
 * @details
 * Can be used as a mask to combine different categories, e.g.:
 * \code
 * auto reservedOrUser = KeywordCategory::Reserved | KeywordCategory::User;
 * auto allButComments = KeywordCategory::All & ~KeywordCategory::Comment;
 * \endcode
 */
class KeywordCategory {

public:
  static const KeywordCategory Mandatory; ///< Mandatory standard keyword.
  static const KeywordCategory Reserved; ///< Optional standard keywords excluding COMMENT and HISTORY.
  static const KeywordCategory Comment; ///< COMMENT and HISTORY keywords (non-valued reserved keywords).
  static const KeywordCategory User; ///< User-defined keywords.
  static const KeywordCategory None; ///< No keyword.
  static const KeywordCategory All; ///< All keywords.

protected:
  /**
   * @brief Constructor.
   */
  explicit KeywordCategory(int category);

public:
  /**
   * @brief Keep only keywords of given categories.
   * @param keywords The keyword vector to be filtered
   * @param categories The categories to be kept, e.g. `KeywordCategory::Reserved | KeywordCategory::User`.
   * @see KeywordCategory
   */
  static std::vector<std::string>
  filterCategories(const std::vector<std::string>& keywords, KeywordCategory categories);

  /**
   * @brief Check whether a keyword is of any of the given categories.
   * @param keyword The keyword to be tested
   * @param categories The categories to be tested, e.g. `KeywordCategory::Reserved | KeywordCategory::User`.
   * @see KeywordCategory
   */
  static bool belongsCategories(const std::string& keyword, KeywordCategory categories);

  /**
   * @brief Check whether a test keyword matches a reference keyword.
   * @details
   * For indexed keywords, the reference keyword is expeced to end with an 'n' character,
   * which represents any positive integer.
   * For example:
   * - `matches("KEY", "KEY")` is true;
   * - `matches("KEY", "KEYn")` is false;
   * - `matches("KEYn", "KEYn")` is true;
   * - `matches("KEY123", "KEYn")` is true;
   * - `matches("KEYn", "KEY123")` is false;
   * - `matches("KEYWORD", "KEYn")` is false.
   */
  static bool matches(const std::string& test, const std::string& ref);

  /**
   * @brief Check equality.
   */
  bool operator==(KeywordCategory rhs) const {
    return m_category == rhs.m_category;
  }

  /**
   * @brief Check inequality.
   */
  bool operator!=(KeywordCategory rhs) const {
    return m_category != rhs.m_category;
  }

  /**
   * @brief Bit-wise OR operator for masking.
   */
  KeywordCategory operator|(KeywordCategory rhs) const {
    KeywordCategory res(*this);
    res |= rhs;
    return res;
  }

  /**
   * @brief In-place bit-wise OR operator for masking.
   */
  KeywordCategory& operator|=(KeywordCategory rhs) {
    m_category |= rhs.m_category;
    return *this;
  }

  /**
   * @brief Bit-wise AND operator for masking.
   */
  KeywordCategory operator&(KeywordCategory rhs) const {
    KeywordCategory res(*this);
    res &= rhs;
    return res;
  }

  /**
   * @brief In-place bit-wise AND operator for masking.
   * @see KeywordCategory
   */
  KeywordCategory& operator&=(KeywordCategory rhs) {
    m_category &= rhs.m_category;
    return *this;
  }

  /**
   * @brief Bit-wise binary NOT operator for masking.
   * @see KeywordCategory
   */
  KeywordCategory operator~() const {
    return KeywordCategory(~m_category);
  }

  /**
   * @brief Cast to bool.
   * @return False for KeywordCategory::None; true otherwise.
   */
  operator bool() const {
    return m_category;
  }

private:
  /**
   * @brief Check whether a test keyword matches an indexed reference keyword.
   * @details
   * The reference keyword is expected to end with an 'n' character,
   * which represents any positive integer.
   */
  static bool matchesIndexed(const std::string& test, const std::string& ref);

  /**
   * @brief Check whether a test keyword matches one of reference keywords.
   */
  static bool matchesOneOf(const std::string& test, const std::vector<std::string>& refs);

  /**
   * @brief The map between standard categories and standard keywords.
   */
  static const std::map<int, const std::vector<std::string>&> byCategory();

  /**
   * @brief The list of mandatory keywords.
   */
  static const std::vector<std::string> m_mandatories;

  /**
   * @brief The list of valued reserved keywords (COMMENT and HISTORY keywords excluded).
   */
  static const std::vector<std::string> m_reserveds;

  /**
   * @brief The list of comment keywords.
   */
  static const std::vector<std::string> m_comments;

  /**
   * @brief The category.
   */
  int m_category;
};

} // namespace Fits
} // namespace Euclid

#endif
