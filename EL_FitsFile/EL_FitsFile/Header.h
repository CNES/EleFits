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

#ifndef _EL_FITSFILE_HEADER_H
#define _EL_FITSFILE_HEADER_H

#include "EL_FitsData/DataUtils.h"
#include "EL_FitsData/Record.h"
#include "EL_FitsData/RecordVector.h"
#include "EL_FitsData/StandardKeyword.h"

#include <fitsio.h>
#include <string>
#include <tuple>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
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
 * @brief Reader-writer for the header unit.
 * @details
 * This class provides services to read and write records in the header units.
 * Several groups of methods are available:
 * - readXxx methods read the raw ASCII characters from the header unit as `std::string`s;
 * - parseXxx methods parse the contents of the header unit in various types;
 * - writeXxx methods write provided values following a strategy defined as a RecordMode.
 * 
 * When reading or writing several records, it is recommended to use the xxxSeq form of the methods
 * (e.g. one call to `writeSeq()` instead of several calls to `write()`), which are optimized.
 * 
 * To write sequences of records, the following types are accepted,
 * as well as their constant and reference counterparts:
 * - For homogeneous sequences:
 *   - `std::vector<Record<T>>` (where `T` can be `VariantValue`),
 *   - `RecordVector<T>` (where `T` can be `VariantValue`),
 *   - `std::array<Record<T>>` (where `T` can be `VariantValue`);
 * - For heterogeneous sequences:
 *   - `Record<Ts>...`,
 *   - `std::tuple<Record<Ts>...>`.
 * 
 * For reading, the following return types are available:
 * - For homogeneous sequences:
 *   - `std::vector<Record<T>>` (where `T` can be `VariantValue`),
 *   - `RecordVector<T>` (where `T` can be `VariantValue`);
 * - For heterogeneous sequences:
 *   - `std::tuple<Record<Ts>...>`,
 *   - User-defined structures which can be constructed from brace-enclosed heterogeneous lists.
 *     Such stuctures should be constructible from brace-enclosed lists of the form `{ Record<Ts>... }` or `{ Ts... }`.
 *     More details and examples are provided in the methods documentation.
 * 
 * Relying on `VariantValue` is the way to go when types are not all known at compile time,
 * and can be the most comfortable option in many other cases.
 * Indeed, working with a tuple might become a nightmare with many values,
 * where `std::vector<VariantValue>` and `RecordVector<VariantValue>` can provide valuable help by reducing the boilerplate.
 * The impact on runtime is negligible.
 */
class Header {

private:
  //// @cond INTERNAL
  friend class RecordHdu;
  //// @endcond

  /**
   * @brief Constructor.
   */
  Header(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @name Read/parse the keywords/records of given categories
   * @param categories The set of selected categories, e.g. `KeywordCategory::Reserved | KeywordCategory::User`
   * @details
   * Example usages:
   * \code
   * auto keywords = h.readKeywords(~KeywordCategory::Comment);
   * auto keyVals = h.readKeywordValues();
   * auto header = h.readAll();
   * auto records = h.parseAll(KeywordCategory::Reserved);
   * \endcode
   * where `h` is a `Header`.
   */
  /// @{

  /**
   * @brief List keywords.
   */
  std::vector<std::string> readKeywords(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief List keywords and their values.
   * @warning
   * If several records have the same keywords, the returned value is a line break-separated list.
   */
  std::map<std::string, std::string> readKeywordsValues(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Read the whole header as a single string.
   * @param categories Either `KeywordCategory::All`, or `~KeywordCategory::Comment` to skip COMMENT and HISTORY records
   */
  std::string readAll(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Read all or a subset of the header records.
   * @warning
   * Comment records are not parsed, as of today.
   */
  RecordVector<VariantValue> parseAll(KeywordCategory categories = KeywordCategory::All) const;

  /// @}
  /**
   * @name Parse a single record, optionally with a fallback
   * @tparam T The record value type
   * @param keyword The record keyword
   * @param fallback The fallback record, keyword of which is looked for;
   * If the keyword is not found, then the record is returned
   * 
   * @details
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
   */
  /// @{

  /**
   * @brief Check whether the HDU contains a given keyword.
   */
  bool has(const std::string& keyword) const;

  /**
   * @brief Parse a record.
   */
  template <typename T>
  Record<T> parse(const std::string& keyword) const;

  /**
   * @brief Parse a record if it exists, return a fallback record otherwise.
   */
  template <typename T>
  Record<T> parseOr(const Record<T>& fallback) const;

  /**
   * @brief Parse a record if it exists, return a fallback otherwise.
   */
  template <typename T>
  Record<T> parseOr(
      const std::string& keyword,
      const T& fallbackValue,
      const std::string& fallbackUnit = "",
      const std::string& fallbackComment = "") const;

  /// @}
  /**
   * @name Parse a sequence of records as a vector or a tuple, optionally with fallbacks
   * @tparam T The record value type for homogeneous sequences (automatically deduced with fallbacks)
   * @tparam Ts The record value types for heterogeneous sequences (automatically deduced with fallbacks)
   * @tparam TSeq The record sequence type (automatically deduced)
   * @param keywords The keywords
   * @param fallbacks The fallback records, keywords of which are looked for
   * @details
   * Example usage with no fallback:
   * \code
   * auto records = h.parseSeq(Named<int>("INT"), Named<float>("FLOAT"));
   * \endcode
   * where `h` is a `Header`
   * 
   * Example usage without fallbacks:
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
   * // RecordVector to RecordVector
   * RecordVector<VariantValue> fallbacks { { {"ONE", 1}, {"TWO", 2.0} } };
   * auto recVec = h.parseSeqOr(fallbacks);
   * 
   * // std::tuple to std::tuple
   * auto fallbacks = std::make_tuple(Record<int>("ONE", 1), Record<double>("TWO", 2.0));
   * auto tuple = h.parseSeqOr(fallbacks);
   * 
   * // Variadic to std::tuple
   * auto tuple = h.parseSeqOr(Record<int>("INT", 0), Record<float>("FLOAT", 3.14));
   * \endcode
   */
  /// @{

  /**
   * @brief Parse a sequence of homogeneous records.
   */
  template <typename T = VariantValue>
  RecordVector<T> parseSeq(const std::vector<std::string>& keywords) const;

  /**
   * @brief Parse a sequence of heterogeneous records.
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeq(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   */
  template <typename TSeq>
  TSeq parseSeqOr(TSeq&& fallbacks) const;

  /**
   * @brief Parse a heterogeneous sequence of records if they exist, return fallbacks for those which don't.
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeqOr(const Record<Ts>&... fallbacks) const;

  /// @}
  /**
   * @name Parse a sequence of records as a user-defined structure, optionally with fallbacks
   * @tparam TOut A structure which can be constructed as:
   * \code TOut { Ts ... } \endcode
   * or:
   * \code TOut { Record<Ts> ... } \endcode
   * like a simple structure:
   * \code struct TOut { T1 p1; T2 p2; ... }; \endcode
   * or a class with such a constructor:
   * \code TOut::TOut(T1, T2, ...) \endcode
   * @tparam Ts The desired record value types (automatically deduced)
   * @tparam TSeq The record sequence type (automatically deduced)
   * @details
   * The output structure can be used to mimic a named tuple,
   * which is generally more convenient than a `std::tuple`,
   * because you access the records as parameters of your own class
   * instead of accessing them by their indices -- with `std::get<i>(tuple)`.
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
   */
  /// @{

  /**
   * @brief Parse a sequence of records.
   */
  template <typename TOut, typename... Ts>
  TOut parseStruct(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   */
  template <typename TOut, typename... Ts>
  TOut parseStructOr(const Record<Ts>&... fallbacks) const;

  /**
   * @brief Parse a sequence of records if they exist, return fallbacks for those which don't.
   */
  template <typename TOut, typename TSeq>
  TOut parseStructOr(TSeq&& fallbacks) const;

  /// @}
  /**
   * @name Write a single record
   * @tparam Mode The write mode
   * @tparam T The record value type (automatically deduced)
   * @param record The record to be written
   * @details
   * Example usages:
   * \code
   * h.write(record);
   * h.write<RecordMode::CreateNew>(record);
   * h.write("KEY", 0);
   * h.write<RecordMode::CreateNew>("KEY", 0);
   * \endcode
   * where `h` is a `Header` and `record` is a `Record<T>`.
   * @see RecordMode
   */
  /// @{

  /**
   * @brief Write a record.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const Record<T>& record) const;

  /**
   * @brief Write a record.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const std::string& k, const T& v, const std::string& u = "", const std::string& c = "") const;

  /**
   * @brief Delete a record.
   * @throw KeywordNotFoundError if the keyword does not exist
   */
  void remove(const std::string& keyword) const;

  /// @}
  /**
   * @name Write a sequence of records
   * @tparam Mode The write mode
   * @tparam T The record value type for homogeneous sequences (automatically deduced)
   * @tparam Ts The record value types for heterogeneous sequences (automatically deduced)
   * @tparam TSeq The sequence type
   * @param records The sequence of records
   * @param keywords The selection to be written
   * @details
   * Example usage:
   * \code
   * h.write(r0, r1, r2);
   * h.write<RecordMode::CreateNew>(r0, r1, r2);
   * \endcode
   * where `h` is a `Header` and `r0`, `r1`, `r2` are `Record<T>`s.
   * @see RecordMode
   */
  /// @{

  /**
   * @brief Write a sequence of records.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeq(const Record<Ts>&... records) const;

  /**
   * @brief Write a sequence of records.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeq(TSeq&& records) const;

  /**
   * @brief Write a subset of a tuple of records.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeqIn(const std::vector<std::string>& keywords, const Record<Ts>&... records) const;

  /**
   * @brief Write a subset of a sequence of records.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeqIn(const std::vector<std::string>& keywords, TSeq&& records) const;

  /// @}
  /**
   * @name Handle checksums
   * @details
   * Two checksums are computed: at whole HDU level (keyword `CHECKSUM`), and at data unit level (keyword `DATASUM`). 
   */
  /// @{

  /**
   * @brief Compute the HDU and data checksums and compare them to the values in the header.
   * @throw ChecksumError if checksums values in header are missing or incorrect
   */
  void verifyChecksums() const;

  /**
   * @brief Compute and write (or update) the HDU and data checksums.
   */
  void updateChecksums() const;

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
 * @brief Exception thrown when a keyword already exists.
 * @ingroup exceptions
 */
struct KeywordExistsError : public FitsIOError {

  /**
   * @brief Constructor.
   */
  KeywordExistsError(const std::string& keyword);

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
 * @brief Exception thrown when a keyword is not found.
 * @ingroup exceptions
 */
struct KeywordNotFoundError : public FitsIOError {

  /**
   * @brief Constructor.
   */
  KeywordNotFoundError(const std::string& keyword);

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

} // namespace FitsIO
} // namespace Euclid

//// @cond INTERNAL
#define _EL_FITSFILE_HEADER_IMPL
#include "EL_FitsFile/impl/Header.hpp"
#undef _EL_FITSFILE_HEADER_IMPL
//// @endcond

#endif
