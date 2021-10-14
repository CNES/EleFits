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

#ifndef _ELEFITS_HEADER_H
#define _ELEFITS_HEADER_H

#include "EleFitsData/DataUtils.h"
#include "EleFitsData/KeywordCategory.h"
#include "EleFitsData/Record.h"
#include "EleFitsData/RecordVec.h"

#include <fitsio.h>
#include <string>
#include <tuple>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @ingroup header_handlers
 * @brief Record writing modes.
 */
enum class RecordMode
{
  CreateOrUpdate, ///< Modify a record if keyword already exists, create a record otherwise
  CreateUnique, ///< Create a record, throw KeywordExistsError if keyword already exists
  CreateNew, ///< Create a new record, even if keyword already exists
  UpdateExisting ///< Modify a record, throw KeywordNotFoundError if keyword doesn't exist
};

/**
 * @ingroup header_handlers
 * @brief Reader-writer for the header unit.
 * 
 * @details
 * This class provides services to read and write records in the header units.
 * Several groups of methods are available:
 * - `read`-prefixed methods read the raw ASCII characters from the header unit as `std::string`s;
 * - In addition, `parse`-prefixed methods parse the values of the records;
 * - `write`-prefixed methods write provided values following a strategy defined as a `RecordMode` enumerator.
 * 
 * When reading or writing several records, it is recommended to use the `Seq`-suffixed methods
 * (e.g. one call to `writeSeq()` instead of several calls to `write()`), which are optimized.
 * 
 * To write sequences of records, the following types are accepted,
 * as well as their constant and reference counterparts:
 * - For homogeneous sequences:
 *   - `std::vector<Record<T>>` (where `T` can be `VariantValue`),
 *   - `RecordVec<T>` (where `T` can be `VariantValue`),
 *   - `std::array<Record<T>>` (where `T` can be `VariantValue`);
 * - For heterogeneous sequences:
 *   - `Record<Ts>...`,
 *   - `std::tuple<Record<Ts>...>`.
 * 
 * For reading, the following return types are available:
 * - For homogeneous sequences:
 *   - `std::vector<Record<T>>` (where `T` can be `VariantValue`),
 *   - `RecordVec<T>` (where `T` can be `VariantValue`);
 * - For heterogeneous sequences:
 *   - `std::tuple<Record<Ts>...>`,
 *   - User-defined structures which can be constructed from brace-enclosed heterogeneous lists.
 *     Such stuctures should be constructible from brace-enclosed lists of the form `{ Record<Ts>... }` or `{ Ts... }`.
 *     More details and examples are provided in the methods documentation.
 * 
 * Relying on `VariantValue` is the way to go when types are not all known at compile time,
 * and can be the most comfortable option in many other cases.
 * Indeed, working with a tuple might become a nightmare with many values,
 * where `std::vector<VariantValue>` and `RecordVec<VariantValue>` (aka `RecordSeq`)
 * can provide valuable help by reducing the boilerplate.
 * The impact on runtime is negligible.
 * 
 * @note
 * As specified in the Fits definition, duplicated keywords lead to an undefined behavior.
 */
class Header {

private:
  friend class Hdu;

  /**
   * @brief Constructor.
   */
  Header(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @name Read the records of given categories
   */
  /// @{

  /**
   * @brief List keywords.
   * 
   * @details
   * Read or parse keywords or records
   * depending on their categories: mandatory, reserved, user, or comment.
   * 
   * Categories can be combined, e.g.:
   * \code
   * auto categories = KeywordCategory::Reserved | KeywordCategory::User;
   * \endcode
   * 
   * Example usages:
   * \code
   * auto keywords = h.readKeywords(~KeywordCategory::Comment);
   * auto keyVals = h.readKeywordValues();
   * auto header = h.readAll();
   * auto records = h.parseAll(KeywordCategory::Reserved);
   * \endcode
   * where `h` is a `Header`.
   * 
   * @param categories The set of selected categories
   * 
   * @see KeywordCategory
   */
  std::vector<std::string> readKeywords(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief List keywords and their values.
   * @copydetails readKeywords()
   * @warning
   * If several records have the same keywords, the returned value is a line break-separated list.
   */
  std::map<std::string, std::string> readKeywordsValues(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Read the whole header as a single string.
   * @copydetails readKeywords()
   * @param categories Either `KeywordCategory::All`, or `~KeywordCategory::Comment` to skip COMMENT and HISTORY records
   */
  std::string readAll(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Parse records of given categories.
   * @copydetails readKeywords()
   * @warning
   * Comment records are not parsed, as of today.
   */
  RecordSeq parseAll(KeywordCategory categories = KeywordCategory::All) const;

  /// @}
  /**
   * @name Read a single record
   */
  /// @{

  /**
   * @brief Check whether the HDU contains a given keyword.
   */
  bool has(const std::string& keyword) const;

  /**
   * @details
   * There are two ways to parse a record: with or without a fallback,
   * that is, a record which is returned if the specified keyword is not found in the header.
   * Without a fallback, the expected return value type is provided as the template parameter.
   * When a fallback is provided, the return type is the fallback type, by default,
   * and the template parameter can be omitted.
   * 
   * Example usages:
   * \code
   * // Parse a record
   * Record<int> record = h.parse<int>("INT");
   * 
   * // Parse a record and keep the value only
   * int value = h.parse<int>("INT");
   * 
   * // Parse a record if available, or get a fallback value
   * auto record = h.parseOr<int>("INT", -1);
   * 
   * // The above line is a shortcut for
   * Record<int> record("INT", -1);
   * if (h.has(record.keyword)) {
   *   record = h.parse<T>(record.keyword);
   * }
   * \endcode
   * where `h` is a `Header`.
   * 
   * @tparam T The record value type
   * @param keyword The record keyword
   * @param fallback The fallback record, keyword of which is looked for
   */

  /**
   * @brief Parse a record.
   */
  template <typename T>
  Record<T> parse(const std::string& keyword) const;

  /**
   * @brief Parse a record if it exists, return a fallback record otherwise.
   * @copydetails parse()
   */
  template <typename T>
  Record<T> parseOr(const Record<T>& fallback) const;

  /**
   * @copydoc parseOr()
   */
  template <typename T>
  Record<T> parseOr(
      const std::string& keyword,
      T fallbackValue,
      const std::string& fallbackUnit = "",
      const std::string& fallbackComment = "") const;

  /// @}
  /**
   * @name Read a sequence of records as a vector or a tuple
   */
  /// @{

  /**
   * @details
   * Like for single record reading, there are two ways to parse a sequence of records:
   * with or without fallbacks.
   * For each record to be parsed, if the specified keyword is not found in the header,
   * the fallback is returned.
   * 
   * When working with fallbacks, the return type is the same as the type of `fallbacks`.
   * 
   * Example usages without fallbacks:
   * \code
   * // Homogeneous records
   * auto vector = h.parseSeq<int>({ "A", "B", "C" });
   * 
   * // Heterogeneous records
   * auto tuple = h.parseSeq(Named<int>("INT"), Named<float>("FLOAT"));
   * \endcode
   * 
   * Example usages with fallbacks:
   * \code
   * // std::vector to std::vector
   * std::vector<Records<VariantValue>> fallbacks { {"ONE", 1}, {"TWO", 2.0} };
   * auto vector = h.parseSeqOr(fallbacks);
   * 
   * // RecordVec to RecordVec
   * RecordSeq fallbacks { { {"ONE", 1}, {"TWO", 2.0} } };
   * auto recVec = h.parseSeqOr(fallbacks);
   * 
   * // std::tuple to std::tuple
   * auto fallbacks = std::make_tuple(Record<int>("ONE", 1), Record<double>("TWO", 2.0));
   * auto tuple = h.parseSeqOr(fallbacks);
   * 
   * // Variadic to std::tuple
   * auto tuple = h.parseSeqOr(Record<int>("INT", 0), Record<float>("FLOAT", 3.14));
   * \endcode
   * 
   * @tparam T The record value type for homogeneous sequences (automatically deduced with fallbacks)
   * @tparam Ts... The record value types for heterogeneous sequences (automatically deduced with fallbacks)
   * @tparam TSeq The record sequence type (automatically deduced)
   * @param keywords The keywords
   * @param fallbacks The fallback records, keywords of which are looked for
   */

  /** 
   * @brief Parse a sequence of homogeneous records.
   */
  template <typename T = VariantValue>
  RecordVec<T> parseSeq(const std::vector<std::string>& keywords) const;

  /**
   * @brief Parse a sequence of heterogeneous records.
   * @copydetails parseSeq()
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeq(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   * @copydetails parseSeq()
   */
  template <typename TSeq>
  TSeq parseSeqOr(TSeq&& fallbacks) const;

  /**
   * @brief Parse a heterogeneous sequence of records if they exist, return fallbacks for those which don't.
   * @copydetails parseSeq()
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeqOr(const Record<Ts>&... fallbacks) const;

  /// @}
  /**
   * @name Read a sequence of records as a user-defined structure
   */
  /// @{

  /**
   * @brief Parse a sequence of records.
   * 
   * @details
   * Several methods are provided to return records or record values as a user-defined structure,
   * instead of a `std::vector` or `std::tuple`.
   * These methods differe from `parseSeq`-prefixed methods in that the returned sequence
   * is interpretted as a user-defined structure,
   * provided that it can be constructed from a brace-enclosed list of `Record`s
   * or from a brace-enclosed list of record values.
   * For example, the return type can be a mere structure like:
   * \code
   * struct TOut {
   *   T0 p0;
   *   T1 p1;
   *   T2 p2;
   * };
   * \endcode
   * or a class with such a constructor:
   * \code
   * TOut::TOut(T0 p0, T1 p1, T2 p2)
   * \endcode
   * where `T0`, `T1`, `T2` are record value types or `Record`s.
   * 
   * The output structure can be used to mimic a named tuple,
   * which is generally more convenient than a `std::tuple`,
   * because the records or values are accessed as named parameters -- e.g. `tout.p1` --
   * instead of being accessed by their indices -- e.g. `std::get<1>(tout)`.
   *
   * Example usage:
   * \code
   * struct Body {
   *   std::string name;
   *   int age;
   *   float height;
   *   float mass;
   *   float bmi() const { return mass / (height * height); }
   * };
   * // Body can be constructed from a brace-enclosed list:
   * // Body body { name, age, height, mass };
   *
   * auto body = hdu.parseStruct<Body>(
   *     Named<std::string>("NAME"),
   *     Named<int>("AGE"),
   *     Named<float>("HEIGHT"),
   *     Named<float>("MASS"));
   *
   * std::cout << "Hello, " << body.name << "!" << std::endl;
   * std::cout << "Your BMI is: " << body.bmi() << std::endl;
   * \endcode
   * 
   * @tparam TOut The return type
   * @tparam Ts... The desired record value types (automatically deduced)
   * @tparam TSeq The record sequence type (automatically deduced)
   */
  template <typename TOut, typename... Ts>
  TOut parseStruct(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   * @copydoc parseStruct()
   */
  template <typename TOut, typename... Ts>
  TOut parseStructOr(const Record<Ts>&... fallbacks) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   * @copydoc parseStruct()
   */
  template <typename TOut, typename TSeq>
  TOut parseStructOr(TSeq&& fallbacks) const;

  /// @}
  /**
   * @name Write a single record
   */
  /// @{

  /**
   * @details
   * Methods to write records may have different behaviors,
   * according to the template parameter `Mode`.
   * It specifies what to do if a keyword already exists (update or throw)
   * and if a keyword does not exist (write or throw).
   * 
   * Example usages:
   * \code
   * h.write(record);
   * h.write<RecordMode::CreateNew>(record);
   * h.write("KEY", 0);
   * h.write<RecordMode::CreateNew>("KEY", 0);
   * \endcode
   * where `h` is a `Header` and `record` is a `Record<T>`.
   * 
   * @tparam Mode The write mode
   * @tparam T The record value type (automatically deduced)
   * @param record The record to be written
   * 
   * @see RecordMode
   */

  /**
   * @brief Write a record.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const Record<T>& record) const;

  /**
   * @copydoc write()
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const std::string& k, T v, const std::string& u = "", const std::string& c = "") const;

  /**
   * @brief Delete a record.
   * @throw KeywordNotFoundError if the keyword does not exist
   */
  void remove(const std::string& keyword) const;

  /// @}
  /**
   * @name Write a sequence of records
   */
  /// @{

  /**
   * @details
   * Several methods allow write a sequence of records or subset of sequence of records.
   * Analogously to `write()`, template parameter `Mode` controls the writing behavior,
   * depending on wether the keyword to be written already exists or not.
   * 
   * If parameter `keywords` is provided, then only the records
   * for which the keyword belongs to `keywords` are written.
   * This is especially handy when a unique sequence of records
   * should be written in different HDUs.
   * 
   * Example usage:
   * \code
   * h0.writeSeq(records);
   * h1.writeSeqIn<RecordMode::CreateNew>({"A", "B"}, records);
   * h2.writeSeqIn<RecordMode::CreateNew>({"B", "C"}, records);
   * \endcode
   * where `h0`, `h1`, `h2` are headers of different HDUs and `records` is a sequence of records,
   * like a `RecordSeq`.
   * 
   * @tparam Mode The write mode
   * @tparam T The record value type for homogeneous sequences (automatically deduced)
   * @tparam Ts... The record value types for heterogeneous sequences (automatically deduced)
   * @tparam TSeq The sequence type (automatically deduced)
   * @param records The sequence of records
   * @param keywords The selection to be written
   * 
   * @see RecordMode
   */

  /**
   * @brief Write a homogeneous sequence of records.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeq(const Record<Ts>&... records) const;

  /**
   * @brief Write a homogeneous or heterogeneous sequence of records.
   * @copydetails writeSeq()
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeq(TSeq&& records) const;

  /**
   * @brief Write a subset of a heterogeneous sequence of records.
   * @copydetails writeSeq()
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeqIn(const std::vector<std::string>& keywords, const Record<Ts>&... records) const;

  /**
   * @brief Write a subset of a homogeneous or heterogeneous sequence of records.
   * @copydetails writeSeq()
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeqIn(const std::vector<std::string>& keywords, TSeq&& records) const;

  /// @}
  /**
   * @name Write comment records.
   */
  /// @{

  /**
   * @brief Write a `COMMENT` record.
   */
  void writeComment(const std::string& comment) const;

  /**
   * @brief Write a `HISTORY` record.
   */
  void writeHistory(const std::string& history) const;

  /// @}

private:
  /**
   * @brief The fitsfile.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The function to declare that the header was touched.
   */
  std::function<void(void)> m_touch;

  /**
   * @brief The function to declare that the header was edited.
   */
  std::function<void(void)> m_edit;
};

/**
 * @ingroup exceptions
 * @brief Exception thrown when a keyword already exists.
 */
struct KeywordExistsError : public FitsError {

  /**
   * @brief Constructor.
   */
  explicit KeywordExistsError(const std::string& keyword);

  /**
   * @brief Throw if an HDU already contains a given keyword.
   */
  static void mayThrow(const std::string& keyword, const Header& header);

  /**
   * @brief Throw if an HDU already contains any of given keywords.
   */
  static void mayThrow(const std::vector<std::string>& keywords, const Header& header);

  /**
   * @brief The keyword which already exists.
   */
  std::string keyword;
};

/**
 * @ingroup exceptions
 * @brief Exception thrown when a keyword is not found.
 */
struct KeywordNotFoundError : public FitsError {

  /**
   * @brief Constructor.
   */
  explicit KeywordNotFoundError(const std::string& keyword);

  /**
   * @brief Throw if an HDU misses a given keyword.
   */
  static void mayThrow(const std::string& keyword, const Header& header);

  /**
   * @brief Throw if an HDU misses any of given keywords.
   */
  static void mayThrow(const std::vector<std::string>& keywords, const Header& header);

  /**
   * @brief The missing keyword.
   */
  std::string keyword;
};

} // namespace Fits
} // namespace Euclid

//// @cond INTERNAL
#define _ELEFITS_HEADER_IMPL
#include "EleFits/impl/Header.hpp"
#undef _ELEFITS_HEADER_IMPL
//// @endcond

#endif
