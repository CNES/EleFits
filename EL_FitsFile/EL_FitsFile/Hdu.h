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

#ifndef _EL_FITSFILE_HDU_H
#define _EL_FITSFILE_HDU_H

#include "EL_FitsData/DataUtils.h"
#include "EL_FitsData/HduCategory.h"
#include "EL_FitsData/KeywordCategory.h"
#include "EL_FitsData/Record.h"
#include "EL_FitsData/RecordVector.h"
#include "EL_FitsFile/Header.h"

#include <fitsio.h>
#include <memory>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup header_handlers
 * @brief Base class for `ImageHdu` and `BintableHdu`.
 * @details
 * This class provides services common to all HDUs for reading and writing records.
 * Services exclusively applicable to MEF files (e.g. HDU name or type) are directly provided as methods,
 * while generic services are accessed through the `header()` method (refer to the documentation of the `Header` class).
 *
 * @note
 * `Hdu`s are written as Image HDUs with `NAXIS` = 0.
 */
class Hdu {

public:
  /// @cond INTERNAL

  /**
   * @brief A token for the passkey idiom.
   * @details
   * Only few classes should be able to create a Hdu.
   * This is enforced by the private Token constructor with restricted set of friend classes.
   */
  class Token {
    friend class MefFile;
    friend class SifFile;
    friend class Hdu;
    friend class ImageHdu;
    friend class BintableHdu;

  private:
    Token() {}
  };

  /**
   * @brief Constructor.
   * @warning
   * You should not try to instantiate a Hdu yourself,
   * but use the dedicated MefFile creation methods.
   * @details
   * The constructor cannot be protected, because unique pointers are created with the make_unique function,
   * and we definitely don't want make_unique to be a friend!
   * We rely on the passkey idiom: Token is protected and therefore accessible only from MefFile (a fiend class)
   * and classes derived from RecrodHdu.
   */
  Hdu(Token,
      fitsfile*& file,
      long index,
      HduCategory type = HduCategory::Image,
      HduCategory status = HduCategory::Untouched);

  /**
   * @brief Dummy constructor, dedicated to iterators.
   */
  Hdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  virtual ~Hdu() = default;

  /**
   * @brief Get the 0-based index of the HDU.
   */
  long index() const;

  /**
   * @brief Get the type of the HDU.
   * @return Either HduCategory::Image or HduCategory::Bintable
   * @details
   * As opposed to readCategory(), the return value of this method can be tested for equality, e.g.:
   * \code
   * if (ext.type() == HduCategory::Image) {
   *   processImage(ext);
   * }
   * \endcode
   */
  HduCategory type() const;

  /**
   * @ingroup iterators
   * @brief Read the category of the HDU.
   * @details
   * This is more specific than the type of the HDU.
   * The category is a bitmask which encodes more properties,
   * e.g. Primary is more specific than Image, and MetadataPrimary is even more specific.
   * The result of this function should not be tested with operator ==, but rather with HduFilter::accepts().
   * Often, the method matches can be used directly.
   * 
   * This is indeed a read operation, because the header should be parsed,
   * e.g. to know whether the data unit is empty or not.
   * @see HduCategory
   * @see matches
   */
  virtual HduCategory readCategory() const;

  /**
   * @ingroup iterators
   * @brief Check whether the HDU matches a given filter.
   * @param filter The list of categories to be tested
   * @warning
   * Like readCategory, this is a read operation.
   */
  bool matches(HduFilter filter) const;

  /**
   * @brief Cast to an ImageHdu or BintableHdu (if possible).
   */
  template <typename T>
  const T& as() const;

  /**
   * @brief Access the header unit to read and write records.
   * @see Header
   */
  const Header& header() const;

  /**
   * @brief Read the extension name.
   */
  std::string readName() const;

  /**
   * @brief Read the extension version.
   */
  long readVersion() const;

  /**
   * @brief Write or update the extension name.
   */
  void updateName(const std::string& name) const;

  /**
   * @brief Write or update the extension version.
   */
  void updateVersion(long version) const;

  /**
   * @brief Read the header as a string.
   * @param incNonValued Include non-valued records (COMMENT, HISTORY, blank).
   * @details
   * The main purpose of this function is to interface with external libraries like WcsLib:
   * \code
   * std::string header = readHeader(false);
   * WcsLib::wcspih(&header[0], header.size()/80, WCSHDR_all, 0, &nreject, &nwcs, &wcs);
   * \endcode
   * @deprecated See header() and Header
   */
  std::string readHeader(bool incNonValued = true) const;

  /**
   * @brief List keywords.
   * @param categories The set of selected categories, e.g. `KeywordCategory::Reserved | KeywordCategory::User`
   * @deprecated See header() and Header
   */
  std::vector<std::string> readKeywords(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief List keywords and their values.
   * @copydetails readKeywords
   * @deprecated See header() and Header
   */
  std::map<std::string, std::string> readKeywordsValues(KeywordCategory categories = KeywordCategory::All) const;

  /**
   * @brief Check whether the HDU contains a given keyword.
   * @deprecated See header() and Header
   */
  bool hasKeyword(const std::string& keyword) const;

  /**
   * @brief Parse a record.
   * @deprecated See header() and Header
   */
  template <typename T>
  Record<T> parseRecord(const std::string& keyword) const;

  /**
   * @brief Parse several records.
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseRecords(const std::vector<std::string>& keywords) const;

  /**
   * @brief Same as parseRecords(const std::vector<std::string> &) with modified signature.
   * @details
   * This allows writing the types along the names, e.g.:
   * \code
   * auto records = parseRecords(Named<int>("INT"), Named<float>("FLOAT"));
   * \endcode
   * instead of:
   * \code
   * auto records = parseRecords<int, float>("INT", "FLOAT");
   * \endcode
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseRecords(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse several records as a user-defined structure.
   * @tparam TReturn A structure which can be constructed as:
   * \code Return { T1, T2, ... } \endcode
   * or:
   * \code Return { Record<T1>, Record<T2>, ... } \endcode
   * like a simple structure:
   * \code struct Return { T1 p1; T2 p2; ... }; \endcode
   * or a class with such constructor:
   * \code Return::Return(T1, T2, ...) \endcode
   * @details
   * This method can be used to mimic a named tuple,
   * which is generally more convenient than a std::tuple,
   * because you chose how to to access the records in your own class
   * instead of accessing them by their indices -- with `std::get<i>(tuple)`.
   *
   * Here's a dummy example to show the usage with a simple structure:
   * \code
   * struct Body {
   *   std::string name;
   *   int age;
   *   float height;
   *   float mass;
   *   float bmi() { return mass / (height * height); }
   * };
   *
   * auto body = hdu.parseRecordsAs<Body, std::string, int, float, float>({ "NAME", "AGE", "HEIGHT", "MASS" });
   *
   * std::cout << "Hello, " << body.name << "!" << std::endl;
   * std::cout << "Your BMI is: " << body.bmi() << std::endl;
   * \endcode
   * @deprecated See header() and Header
   */
  template <class TReturn, typename... Ts>
  TReturn parseRecordsAs(const std::vector<std::string>& keywords) const;

  /**
   * @brief Same as parseRecordsAs(const std::vector<std::string> &) with modified signature.
   * @details
   * As opposed to parseRecordsAs(const std::vector<std::string> &),
   * record types are given along with the names for more safety.
   * For example, instead of:
   * \code
   * auto body = hdu.parseRecordsAs<Body, std::string, int, float, float>({ "NAME", "AGE", "HEIGHT", "MASS" });
   * \endcode
   * write:
   * \code
   * auto body = hdu.parseRecordsAs<Body>(
   *     Named<std::string>("NAME"),
   *     Named<int>("AGE"),
   *     Named<float>("HEIGHT"),
   *     Names<float>("MASS"));
   * \endcode
   * @deprecated See header() and Header
   */
  template <class TReturn, typename... Ts>
  TReturn parseRecordsAs(const Named<Ts>&... keywords) const;

  /**
   * @brief Parse a record if it exists, return a fallback otherwise.
   * @param fallback A record with the keyword to be looked for, the fallback value,
   * a fallback unit (optional), and a fallback comment (optional).
   * @details
   * If a record with the given keyword exists, it is parsed and returned.
   * Otherwise, a copy of the given fallback record is returned.
   * For example, when calling:
   * \code
   * const auto record = hdu.parseRecordOr<int>({"KEY", 0});
   * \endcode
   * if KEY is found, the record is parsed;
   * otherwise, the returned record has keyword "KEY", value 0, empty unit, and empty comment.
   * @deprecated See header() and Header
   */
  template <typename T>
  Record<T> parseRecordOr(const Record<T>& fallback) const;

  /**
   * @brief Parse several records with a fallback.
   * @details
   * For each keyword, if it exists, the record is parsed;
   * otherwise the fallback record is returned.
   * @see parseRecordOr
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseRecordsOr(const Record<Ts>&... fallbacks) const;

  /**
   * @brief Parse several homogeneous records.
   * @tparam T The common value type.
   * @deprecated See header() and Header
   */
  template <typename T>
  RecordVector<T> parseRecordVector(const std::vector<std::string>& keywords) const;

  /**
   * @brief Parse several heterogeneous records.
   * @details
   * The underlying type of each record is deduced from the value in the Fits file.
   * Each record in the collection can later be cast with method RecordSeq::as<T>(),
   * which returns a Record<T>, which can itself be sliced as its value.
   * For example:
   * \code
   * auto records = f.parseRecordSeq({ "INT", "FLOAT", "DOUBLE", "BOOL" });
   * int value = records.as<int>("INT");
   * \endcode
   * @deprecated See header() and Header
   */
  RecordSeq parseRecordSeq(const std::vector<std::string>& keywords) const;

  /**
   * @brief Parse all the records as a RecordVector.
   * @details
   * This method is similar to parseRecordVector.
   * @deprecated See header() and Header
   */
  template <typename T>
  RecordVector<T> parseAllRecords() const;

  /**
   * @brief Write a record.
   */
  template <typename T>
  void writeRecord(const Record<T>& record) const;

  /**
   * @brief Write a record.
   * @param k The keyword.
   * @param v The value.
   * @param u The unit.
   * @param c The comment.
   * @deprecated See header() and Header
   */
  template <typename T>
  void writeRecord(const std::string& k, T v, const std::string& u = "", const std::string& c = "") const;

  /**
   * @brief Write several records.
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  void writeRecords(const Record<Ts>&... records) const;

  /**
   * @brief Write several records.
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  void writeRecords(const std::tuple<Record<Ts>...>& records) const;

  /**
   * @brief Write several homogeneous records.
   * @details
   * Similarly to the reading counterpart parseRecordVector, `T` can also be `VariantValue`.
   * @deprecated See header() and Header
   */
  template <typename T>
  void writeRecords(const std::vector<Record<T>>& records) const;

  /**
   * @brief Write a subset of a RecordVector.
   * @deprecated See header() and Header
   */
  template <typename T>
  void writeRecords(const RecordVector<T>& records, const std::vector<std::string>& keywords) const;

  /**
   * @brief Write a COMMENT record.
   * @deprecated See header() and Header
   */
  void writeComment(const std::string& comment) const;

  /**
   * @brief Write a HISTORY record.
   * @deprecated See header() and Header
   */
  void writeHistory(const std::string& history) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   * @deprecated See header() and Header
   */
  template <typename T>
  void updateRecord(const Record<T>& record) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   * @param k The keyword.
   * @param v The value.
   * @param u The unit.
   * @param c The comment.
   * @deprecated See header() and Header
   */
  template <typename T>
  void updateRecord(const std::string& k, T v, const std::string& u = "", const std::string& c = "") const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  void updateRecords(const Record<Ts>&... records) const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   * @deprecated See header() and Header
   */
  template <typename... Ts>
  void updateRecords(const std::tuple<Record<Ts>...>& records) const;

  /**
   * @brief Update several homogeneous records if they exist; write new records otherwise.
   * @deprecated See header() and Header
   */
  template <typename T>
  void updateRecords(const std::vector<Record<T>>& records) const;

  /**
   * @brief Update a subset of a RecordVector.
   * @deprecated See header() and Header
   */
  template <typename T>
  void updateRecords(const RecordVector<T>& records, const std::vector<std::string>& keywords) const;

  /**
   * @brief Delete a record.
   */
  void deleteRecord(const std::string& keyword) const;

  /**
   * @brief Compute the HDU and data checksums and compare them to the values in the header.
   * @throw ChecksumError if checksums values in header are missing or incorrect
   * @see updateChecksums()
   */
  void verifyChecksums() const;

  /**
   * @brief Compute and write (or update) the HDU and data checksums.
   * @details
   * Two checksums are computed: at whole HDU level (keyword `CHECKSUM`), and at data unit level (keyword `DATASUM`).
   * @see verifyChecksums()
   */
  void updateChecksums() const;

protected:
  /**
   * @brief Set the current HDU to this one.
   * @details
   * The status of the HDU is modified to Touched if it was initially Untouched.
   */
  void touchThisHdu() const;

  /**
   * @brief Set the current HDU to this one for writing.
   * @details
   * Edited is added to the status of the HDU.
   */
  void editThisHdu() const;

  /**
   * @brief The parent file handler.
   * @warning
   * This is a reference to a pointer because we want the pointer to be valid
   * even if file is closed and reopened.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The 1-based CFitsIO HDU index.
   * @warning
   * EL_FitsIO HDUs are 0-based in the public API and 1-based in the CfitsioWrapper namespace
   * because CFitsIO indices are 1-based.
   */
  long m_cfitsioIndex;

  /**
   * @brief The HDU type.
   */
  HduCategory m_type;

  /**
   * @brief The header unit handler.
   */
  Header m_header;

  /**
   * @brief The HDU status.
   * @details
   * It is mutable because read/write operations change it.
   * Another option to avoid such practice would be to store a record of statuses in MefFile instead.
   */
  mutable HduCategory m_status;

  /**
   * @brief Dummy file handler dedicated to dummy constructor.
   */
  fitsfile* m_dummyFptr = nullptr;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_HDU_IMPL
#include "EL_FitsFile/impl/Hdu.hpp"
#undef _EL_FITSFILE_HDU_IMPL
/// @endcond

#endif
