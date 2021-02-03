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

#ifndef _EL_CFITSIOWRAPPER_HEADERWRAPPER_H
#define _EL_CFITSIOWRAPPER_HEADERWRAPPER_H

#include <map>
#include <string>
#include <tuple>
#include <typeinfo> // type_info
#include <vector>

#include <fitsio.h>

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"
#include "EL_FitsData/Record.h"
#include "EL_FitsData/RecordVector.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Header-related functions.
 */
namespace Header {

/**
 * @brief Standard Fits keywords and related utils.
 */
class StandardKeyword {

public:
  /**
   * @brief Keyword categories.
   */
  enum Category {
    Required = 0b0001, ///< Required keywords are standard and mandatory.
    Reserved = 0b0010, ///< Reserved keywords are standard and optional (only valued keywords are put in this category).
    Comment = 0b0100, ///< COMMENT (and HISTORY) keywords are special non-valued reserved keywords.
    User = 0b1000, ///< User-defined keywords.
    None = 0b0000, ///< No keyword.
    All = 0b1111 ///< All keywords.
  };

  /**
   * @brief Keep only keywords of given categories.
   * @param keywords The keyword vector to be filtered
   * @param categories The categories to be kept, e.g. `Category::Reserved | Category::User`.
   */
  static std::vector<std::string> filterCategories(const std::vector<std::string> &keywords, Category categories);

  /**
   * @brief Check whether a keyword is of given categories.
   * @param keywords The keyword vector to be tested
   * @param categories The categories to be tested, e.g. `Category::Reserved | Category::User`.
   */
  static bool belongsCategories(const std::string &keyword, Category categories);

  /**
   * @brief Check whether a keyword is of one given category.
   */
  static bool belongsCategory(const std::string &keyword, Category category);

  /**
   * @brief Check whether a test keyword matches a reference keyword.
   * @details
   * For indexed keywords, the reference keyword is expeced to end with an 'n' character
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
  static const std::map<Category, const std::vector<std::string> &> byCategory();

  /**
   * @brief The list of required keywords.
   */
  static const std::vector<std::string> m_requireds;

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
 * @brief Bit-wise or operator for masking.
 */
inline StandardKeyword::Category operator|(StandardKeyword::Category a, StandardKeyword::Category b) {
  return static_cast<StandardKeyword::Category>(static_cast<int>(a) | static_cast<int>(b));
}

/**
 * @brief In-place bit-wise or operator for masking.
 */
inline StandardKeyword::Category &operator|=(StandardKeyword::Category &a, StandardKeyword::Category b) {
  a = a | b;
  return a;
}

/**
 * @brief Bit-wise and operator for masking.
 */
inline StandardKeyword::Category operator&(StandardKeyword::Category a, StandardKeyword::Category b) {
  return static_cast<StandardKeyword::Category>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * @brief Bit-wise binary not operator for masking.
 */
inline StandardKeyword::Category operator~(StandardKeyword::Category a) {
  return static_cast<StandardKeyword::Category>(~static_cast<int>(a));
}

/**
 * @brief Read the whole header as a string.
 * @param fptr A pointer to the fitsfile object.
 * @param incNonValued Include non-valued records (COMMENT, HISTORY, blank).
 */
std::string readHeader(fitsfile *fptr, bool incNonValued = true);

/**
 * @brief List the keywords of selected categories.
 */
std::vector<std::string>
listKeywords(fitsfile *fptr, StandardKeyword::Category filter = StandardKeyword::Category::All);

/**
 * @brief Check whether the current HDU contains a given keyword.
 */
bool hasKeyword(fitsfile *fptr, const std::string &keyword);

/**
 * @brief Parse a record.
 */
template <typename T>
FitsIO::Record<T> parseRecord(fitsfile *fptr, const std::string &keyword);

/**
 * @brief Parse records.
 */
template <typename... Ts>
std::tuple<FitsIO::Record<Ts>...> parseRecords(fitsfile *fptr, const std::vector<std::string> &keywords);

/**
 * @brief Parse records and store them in a user-defined structure.
 * @tparam TReturn A class which can be brace-initialized with a pack of records or values.
 */
template <class TReturn, typename... Ts>
TReturn parseRecordsAs(fitsfile *fptr, const std::vector<std::string> &keywords);

/**
 * @brief Parse homogeneous records and store them in a vector.
 */
template <typename T>
FitsIO::RecordVector<T> parseRecordVector(fitsfile *fptr, const std::vector<std::string> &keywords);

/**
 * @brief Write a new record.
 */
template <typename T>
void writeRecord(fitsfile *fptr, const FitsIO::Record<T> &record);

/**
 * @brief Write new records.
 */
template <typename... Ts>
void writeRecords(fitsfile *fptr, const FitsIO::Record<Ts> &... records);

/**
 * @brief Write new records.
 */
template <typename... Ts>
void writeRecords(fitsfile *fptr, const std::tuple<FitsIO::Record<Ts>...> &records);

/**
 * @brief Write homogeneous records.
 */
template <typename T>
void writeRecords(fitsfile *fptr, const std::vector<FitsIO::Record<T>> &records);

/**
 * @brief Update an existing record or write a new one.
 */
template <typename T>
void updateRecord(fitsfile *fptr, const FitsIO::Record<T> &record);

/**
 * @brief Update existing records or write new ones.
 */
template <typename... Ts>
void updateRecords(fitsfile *fptr, const FitsIO::Record<Ts> &... records);

/**
 * @brief Update existing records or write new ones.
 */
template <typename... Ts>
void updateRecords(fitsfile *fptr, const std::tuple<FitsIO::Record<Ts>...> &records);

/**
 * @brief Update existing homogeneous records or write new ones.
 */
template <typename T>
void updateRecords(fitsfile *fptr, const std::vector<FitsIO::Record<T>> &records);

/**
 * @brief Delete an existing record.
 */
void deleteRecord(fitsfile *fptr, const std::string &keyword);

/**
 * @brief Get the typeid of a record value.
 */
const std::type_info &recordTypeid(fitsfile *fptr, const std::string &keyword);

/**
 * @brief Write COMMENT record.
 */
void writeComment(fitsfile *fptr, const std::string &comment);

/**
 * @brief Write HISTORY record.
 */
void writeHistory(fitsfile *fptr, const std::string &history);

} // namespace Header
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_HEADERWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/HeaderWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_HEADERWRAPPER_IMPL
/// @endcond

#endif
