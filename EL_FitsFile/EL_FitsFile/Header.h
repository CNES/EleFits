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
  CreateUnique, ///< Create a record, throw if keyword already exists
  CreateNew, ///< Create a record, even if keyword already exists
  UpdateExisting, ///< Modify a record, throw if keyword doesn't exist
  CreateOrUpdate ///< Modify a record if keyword already exists, create a record otherwise
};

/**
 * @brief Header unit reader-writer.
 * @details
 * Several groups of methods are available:
 * - readXxx methods read the raw ASCII characters from the header unit as strings;
 * - parseXxx methods parse the contents of the header unit to return user-specified types;
 * - writeXxx methods write provided values following a strategy defined as a RecordMode.
 * 
 * When reading or writing several records, it is recommended to use the xxxSeq form of the methods
 * (e.g. one call to `writeSeq()` instead of several calls to `write()`), which are optimized.
 * 
 * To write sequences of records, the following parameters are accepted,
 * as well as their constant and reference counterparts:
 * - For homogeneous sequences:
 *   - `std::vector<Record<T>>`,
 *   - `std::array<Record<T>>`;
 * - For heterogeneous sequences:
 *   - `Record<Ts>...`,
 *   - `std::tuple<Record<Ts>...>`,
 *   - `std::vector<Record<VariantValue>>`,
 *   - `std::array<Record<VariantValue>>`.
 * 
 * For reading, the following return types are available:
 * - For homogeneous sequences:
 *   - `RecordVector<T>`;
 * - For heterogeneous sequences:
 *   - `std::tuple<Record<Ts>...>`,
 *   - `RecordVector<VariantValue>`,
 *   - User-specified structures which can be constructed from brace-enclosed heterogeneous lists (TODO document).
 * 
 * Relying on `VariantValue` is the way to go when types are not all known at compile time,
 * and can be the most comfortable option in other cases.
 * Indeed, working with a tuple might become a nightmare with many values,
 * where `std::vector<VariantValue>` and `RecordVector<VariantValue>` can provide valuable help
 * by reducing the boilerplate.
 * The impact on runtime is negligible.
 * 
 * @warning
 * There is a known bug in CFitsIO (TODO version?) with the reading of Record<unsigned long>:
 * if the value is greater than `max(long)`, CFitsIO returns an overflow error.
 * This is a false alarm but cannot be worked around easily.
 * There should be a fix on CFitsIO side.
 */
class Header {

private:
  friend class RecordHdu;

  /**
   * @brief Constructor.
   */
  Header(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @brief Check whether the HDU contains a given keyword.
   */
  bool has(const std::string& keyword) const;

  /**
   * @brief Delete a record.
   * @throw KeywordNotFoundError
   */
  void remove(const std::string& keyword) const;

  /**
   * @brief List keywords.
   * @param categories The set of selected categories, e.g. `KeywordCategory::Reserved | KeywordCategory::User`
   */
  std::vector<std::string> readKeywords(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief List keywords and their values.
   * @param categories The set of selected categories, e.g. `KeywordCategory::Reserved | KeywordCategory::User`
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
   * @param categories The selected keyword categories
   * @details
   * Example usage:
   * \code
   * auto records = h.parseAll(KeywordCategory::Reserved);
   * \endcode
   * where `h` is a `Header`.
   * @warning
   * Comment records are not parsed, as of today.
   */
  RecordVector<VariantValue> parseAll(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Parse a record.
   * @tparam T The record value type
   * @param keyword The record keyword
   * @details
   * Example usages:
   * \code
   * auto record = h.parse<int>("KEY"); // Get a Record<int>
   * int value = h.parse<int>("KEY"); // Get only the value of a Record<int>
   * \endcode
   * where `h` is a `Header`.
   */
  template <typename T>
  Record<T> parse(const std::string& keyword) const;

  /**
   * @brief Parse a record if it exists, return a fallback record otherwise.
   * @tparam T The record value type (automatically deduced)
   * @param fallback The fallback record, keyword of which is looked for
   * @details
   * This is a shortcut for:
   * \code
   * // auto record = h.parseOr(fallback);
   * auto record = fallback;
   * if (h.has(fallback.keyword)) {
   *   record = h.parse<T>(fallback.keyword);
   * }
   * \endcode
   * where `h` is a `Header` and `T` is the value type of the fallback.
   */
  template <typename T>
  Record<T> parseOr(const Record<T>& fallback) const;

  /**
   * @brief Parse a record if it exists, return a fallback otherwise.
   * @tparam T The record value type (automatically deduced)
   * @param keyword The record keyword
   * @param fallbackValue The fallback value
   * @param fallbackUnit The fallback unit
   * @param fallbackComment The fallback comment
   * @details
   * This is a shortcut for:
   * \code
   * // auto record = h.parseOr(keyword, value, unit, comment);
   * Record<T> record(keyword, value, unit, comment);
   * if (h.has(keyword)) {
   *   record = h.parse<T>(keyword);
   * }
   * \endcode
   * where `h` is a `Header` and `T` is the value type of the fallback.
   */
  template <typename T>
  Record<T> parseOr(
      const std::string& keyword,
      const T& fallbackValue,
      const std::string& fallbackUnit = "",
      const std::string& fallbackComment = "") const;

  /**
   * @brief Parse a sequence of homogeneous records.
   * @tparam T The record value type
   * @param keywords The keywords
   */
  template <typename T = VariantValue>
  RecordVector<T> parseSeq(const std::vector<std::string>& keywords) const;

  /**
   * @brief Parse a sequence of heterogeneous records.
   * @tparam Ts The record value types (automatically deduced)
   * @param keywords The keywords and value types of the records to be parsed
   * @details
   * Example usage:
   * \code
   * auto records = h.parseSeq(Named<int>("INT"), Named<float>("FLOAT"));
   * \endcode
   * where `h` is a `Header`
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeq(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a sequence records if they exists, return fallbacks for those which don't.
   * @tparam TSeq A record sequence (automatically deduced)
   * @param fallbacks The fallback records, keywords of which are looked for
   * @return
   * If `TSeq` is an `std::vector`, then a `RecordVector` is returned;
   * If `TSeq` is an `std::tuple`, then an `std::tuple` is returned.
   * @details
   * Example usages:
   * \code
   * std::vector<Records<VariantValue>> records { {"ONE", 1}, {"TWO", 2.0} };
   * auto vector = h.parseSeqOr(records); // Returns a RecordVector<VariantValue>
   * // OR
   * auto records = std::make_tuple(Record<int>("ONE", 1), Record<double>("TWO", 2.0));
   * auto tuple = h.parseSeqOr(records); // Returns an std::tuple<int, double>
   * \endcode
   */
  template <typename TSeq>
  TSeq parseSeqOr(TSeq&& fallbacks) const;

  /**
   * @brief Parse several records if they exists, return fallbacks for those which don't.
   * @tparam TRecords The record types (automatically deduced)
   * @param fallbacks The fallback records, keywords of which are looked for
   * @details
   * Example usage:
   * \code
   * auto records = h.parseSeqOr(Record<int>("INT", 0), Record<float>("FLOAT", 3.14));
   * \endcode
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseSeqOr(const Record<Ts>&... fallbacks) const;

  /**
   * @brief Parse a sequence of heterogeneous records as a user-defined structure.
   * @tparam TReturn A structure which can be constructed as:
   * \code TReturn { Ts ... } \endcode
   * or:
   * \code TReturn { Record<Ts> ... } \endcode
   * like a simple structure:
   * \code struct TReturn { T1 p1; T2 p2; ... }; \endcode
   * or a class with such a constructor:
   * \code TReturn::TReturn(T1, T2, ...) \endcode
   * @tparam Ts The desired record value types (automatically deduced)
   * @details
   * This method can be used to mimic a named tuple,
   * which is generally more convenient than a std::tuple,
   * because you chose how to to access the records in your own class
   * instead of accessing them by their indices -- with `std::get<i>(tuple)`.
   *
   * Example usage:
   * \code
   * // Body can be constructed from a brace-enclosed list:
   * // Body body { name, age, height, mass };
   * struct Body {
   *   std::string name;
   *   int age;
   *   float height;
   *   float mass;
   *   float bmi() const { return mass / (height * height); }
   * };
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
  template <typename TReturn, typename... Ts>
  TReturn parseStruct(const Named<Ts>&... keywords) const;

  template <typename TReturn, typename... Ts>
  TReturn parseStructOr(const Record<Ts>&... fallbacks) const;

  template <typename TReturn, typename TSeq>
  TReturn parseStructOr(TSeq&& fallbacks) const;

  /**
   * @brief Write a record.
   * @tparam Mode The write mode
   * @tparam T The record value type, doesn't need to be specified
   * @details
   * Example usages:
   * \code
   * h.write(record);
   * h.write<RecordMode::CreateNew>(record);
   * \endcode
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const Record<T>& record) const;

  /**
   * @brief Write a record.
   * @tparam Mode The write mode
   * @tparam T The record value type, doesn't need to be specified
   * @details
   * Example usages:
   * \code
   * h.write("KEY", 0);
   * h.write<RecordMode::CreateNew>("KEY", 0);
   * \endcode
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename T>
  void write(const std::string& k, const T& v, const std::string& u = "", const std::string& c = "") const;

  /**
   * @brief Write a tuple of records.
   * @tparam Mode The write mode
   * @tparam Ts The record value types, doesn't need to be specified
   * @param records The records
   * @param mode The write mode
   * @details
   * Example usage:
   * \code
   * h.write(r0, r1, r2);
   * h.write<RecordMode::CreateNew>(r0, r1, r2);
   * \endcode
   * where `h` is a `Header` and `r1`, `r2`, `r3` are `Record`s.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeq(const Record<Ts>&... records) const;

  /**
   * @brief Write a sequence of records.
   * @tparam Mode The write mode
   * @tparam TSeq A record sequence type (automatically deduced)
   * @param records The records
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeq(TSeq&& records) const;

  /**
   * @brief Write a subset of a tuple of records.
   * @tparam Mode The write mode
   * @tparam Ts The record value types (automatically deduced)
   * @param keywords The selection of records to be written
   * @param records The available records
   * @details
   * Example usage:
   * \code
   * h.write({ "R0", "R1" }, r0, r1, r2);
   * h.write<RecordMode::CreateNew>({ "R0", "R1" }, r0, r1, r2);
   * \endcode
   * where `h` is a `Header` and `r0`, `r1`, `r2` are `Record`s.
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename... Ts>
  void writeSeqIn(const std::vector<std::string>& keywords, const Record<Ts>&... records) const;

  /**
   * @brief Write a subset of a tuple of records.
   * @tparam Mode The write mode
   * @tparam TSeq A record sequence type (automatically deduced)
   * @param keywords The selection of records to be written
   * @param records The available records
   * @param mode The write mode
   */
  template <RecordMode Mode = RecordMode::CreateOrUpdate, typename TSeq>
  void writeSeqIn(const std::vector<std::string>& keywords, TSeq&& records) const;

  /**
   * @brief Compute the HDU and data checksums and compare them to the values in the header.
   * @throw ChecksumError if checksums values in header are missing or incorrect
   */
  void verifyChecksums() const;

  /**
   * @brief Compute and write (or update) the HDU and data checksums.
   */
  void updateChecksums() const;

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

/// @cond INTERNAL
#define _EL_FITSFILE_HEADER_IMPL
#include "EL_FitsFile/impl/Header.hpp"
#undef _EL_FITSFILE_HEADER_IMPL
/// @endcond

#endif
