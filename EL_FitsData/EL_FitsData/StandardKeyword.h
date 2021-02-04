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

#ifndef _EL_CFITSIOWRAPPER_STANDARDKEYWORD_H
#define _EL_CFITSIOWRAPPER_STANDARDKEYWORD_H

#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Keyword categories.
 * @details
 * Can be used as a mask to combine different categories, e.g.:
 * \code
 * auto reservedOrUser = KeywordCategory::Reserved | KeywordCategory::User;
 * auto allButComments = KeywordCategory::All & ~KeywordCategory::Comment;
 * \endcode
 */
enum KeywordCategory {
  Mandatory = 0b0001, ///< Mandatory standard keyword.
  Reserved = 0b0010, ///< Optional standard keywords (only valued keywords are put in this category).
  Comment = 0b0100, ///< COMMENT (and HISTORY) keywords (non-valued reserved keywords).
  User = 0b1000, ///< User-defined keywords.
  None = 0b0000, ///< No keyword.
  All = 0b1111 ///< All keywords.
};

/**
 * @brief Standard Fits keywords and related utils.
 */
class StandardKeyword {

public:
  /**
   * @brief Keep only keywords of given categories.
   * @param keywords The keyword vector to be filtered
   * @param categories The categories to be kept, e.g. `KeywordCategory::Reserved | KeywordCategory::User`.
   * @see KeywordCategory
   */
  static std::vector<std::string>
  filterCategories(const std::vector<std::string> &keywords, KeywordCategory categories);

  /**
   * @brief Check whether a keyword is of given categories.
   * @param keywords The keyword vector to be tested
   * @param categories The categories to be tested, e.g. `KeywordCategory::Reserved | KeywordCategory::User`.
   * @see KeywordCategory
   */
  static bool belongsCategories(const std::string &keyword, KeywordCategory categories);

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
  static bool matches(const std::string &test, const std::string &ref);

private:
  /**
   * @brief Check whether a test keyword matches an indexed reference keyword.
   * @details
   * The reference keyword is expected to end with an 'n' character,
   * which represents any positive integer.
   */
  static bool matchesIndexed(const std::string &test, const std::string &ref);

  /**
   * @brief Check whether a test keyword matches one of reference keywords.
   */
  static bool matchesOneOf(const std::string &test, const std::vector<std::string> &refs);

  /**
   * @brief The map between standard categories and standard keywords.
   */
  static const std::map<KeywordCategory, const std::vector<std::string> &> byCategory();

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
};

/**
 * @brief Bit-wise OR operator for masking.
 * @see KeywordCategory
 */
inline KeywordCategory operator|(KeywordCategory a, KeywordCategory b) {
  return static_cast<KeywordCategory>(static_cast<int>(a) | static_cast<int>(b));
}

/**
 * @brief In-place bit-wise OR operator for masking.
 * @see KeywordCategory
 */
inline KeywordCategory &operator|=(KeywordCategory &a, KeywordCategory b) {
  a = a | b;
  return a;
}

/**
 * @brief Bit-wise AND operator for masking.
 * @see KeywordCategory
 */
inline KeywordCategory operator&(KeywordCategory a, KeywordCategory b) {
  return static_cast<KeywordCategory>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * @brief In-place bit-wise AND operator for masking.
 * @see KeywordCategory
 */
inline KeywordCategory &operator&=(KeywordCategory &a, KeywordCategory b) {
  a = a & b;
  return a;
}

/**
 * @brief Bit-wise binary NOT operator for masking.
 * @see KeywordCategory
 */
inline KeywordCategory operator~(KeywordCategory a) {
  return static_cast<KeywordCategory>(~static_cast<int>(a));
}

} // namespace FitsIO
} // namespace Euclid

#endif
