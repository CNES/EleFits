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

#include "EL_CfitsioWrapper/HeaderWrapper.h"
#include "EL_FitsData/Record.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Header reader-writer.
 * @details
 * This class provides services common to all HDUs, including for reading and writing records.
 * When reading or writing several records, it is recommended to use the plural form of the methods
 * (e.g. one call to RecordHdu::writeRecords instead of several calls to RecordHdu::writeRecord),
 * which are optimized.
 * Methods to update records are analogous to methods to read records:
 * refer to the documentation of the latter for more details on the former.
 * @warning
 * There is a known bug in CFitsIO with the reading of Record<unsigned long>:
 * if the value is greater than `max(long)`, CFitsIO returns an overflow error.
 * This is a false alarm but cannot be worked around easily.
 * There should be a fix on CFitsIO side.
 */
class RecordHdu {

public:
  /**
   * @brief Constructor.
   * @warning
   * You should probablt not instantiate `RecordHdu`s yourself,
   * but use the dedicated MefFile creation method MefFile::initRecordExt.
   * @todo
   * The constructor should be protected, with MefFile a friend of the class.
   */
  RecordHdu(fitsfile *&file, long index);

  /**
   * @brief Destructor.
   */
  virtual ~RecordHdu() = default;

  /**
   * @brief Get the 1-based index of the HDU.
   */
  long index() const;

  /**
   * @brief Read the extension name.
   */
  std::string readName() const;

  /**
   * @brief Write or update the extension name.
   */
  void updateName(const std::string &name) const;

  /**
   * @brief List the valued record keywords.
   * @warning
   * Non-valued records, like COMMENT and HISTORY records, are bypassed.
   */
  std::vector<std::string> readKeywords() const;

  /**
   * @brief Parse a record.
   */
  template <typename T>
  Record<T> parseRecord(const std::string &keyword) const;

  /**
   * @brief Parse several records.
   */
  template <typename... Ts>
  std::tuple<Record<Ts>...> parseRecords(const std::vector<std::string> &keywords) const;

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
   * This is generally more convenient than a tuple
   * because you chose how to to access the records in your own class
   * insted of accessing them by their indices -- with `std::get<i>(tuple)`.
   */
  template <class TReturn, typename... Ts>
  TReturn parseRecordsAs(const std::vector<std::string> &keywords) const;

  /**
   * @brief Parse several homogeneous records.
   * @tparam T The common value type.
   * @details
   * In addition to parsing homogeneous records, like a set of integer records,
   * this method can be used with `boost::any` records to allow for runtime type deduction:
   * \code
   * auto records = f.parseRecordVector<boost::any>({ "INT", "FLOAT", "DOUBLE", "BOOL" });
   * int value = records.as<int>("INT");
   * \endcode
   * In this case, the underlying type of each record is deduced from the value in the Fits file.
   */
  template <typename T>
  RecordVector<T> parseRecordVector(const std::vector<std::string> &keywords) const;

  /**
   * @brief Parse all the records as a RecordVector.
   * @details
   * This method is similar to parseRecordVector.
   */
  template <typename T>
  RecordVector<T> parseAllRecords() const;

  /**
   * @brief Write a record.
   */
  template <typename T>
  void writeRecord(const Record<T> &record) const;

  /**
   * @brief Write a record.
   * @param k The keyword
   * @param v The value
   * @param u The unit
   * @param c The comment
   */
  template <typename T>
  void writeRecord(const std::string &k, T v, const std::string &u = "", const std::string &c = "") const;

  /**
   * @brief Write several records.
   */
  template <typename... Ts>
  void writeRecords(const Record<Ts> &... records) const;

  /**
   * @brief Write several records.
   */
  template <typename... Ts>
  void writeRecords(const std::tuple<Record<Ts>...> &records) const;

  /**
   * @brief Write several homogeneous records.
   * @details
   * Similarly to the reading counterpart parseRecordVector, `T` can also be `boost::any`.
   */
  template <typename T>
  void writeRecords(const std::vector<Record<T>> &records) const;

  /**
   * @brief Write a subset of a RecordVector.
   */
  template <typename T>
  void writeRecords(const RecordVector<T> &records, const std::vector<std::string> &keywords) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   */
  template <typename T>
  void updateRecord(const Record<T> &record) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   * @param k The keyword
   * @param v The value
   * @param u The unit
   * @param c The comment
   */
  template <typename T>
  void updateRecord(const std::string &k, T v, const std::string &u = "", const std::string &c = "") const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   */
  template <typename... Ts>
  void updateRecords(const Record<Ts> &... records) const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   */
  template <typename... Ts>
  void updateRecords(const std::tuple<Record<Ts>...> &records) const;

  /**
   * @brief Update several homogeneous records if they exist; write new records otherwise.
   */
  template <typename T>
  void updateRecords(const std::vector<Record<T>> &records) const;

  /**
   * @brief Update a subset of a RecordVector.
   */
  template <typename T>
  void updateRecords(const RecordVector<T> &records, const std::vector<std::string> &keywords) const;

  /**
   * @brief Delete a record.
   */
  void deleteRecord(const std::string &keyword) const;

protected:
  /**
   * @brief Set the current HDU to this one.
   */
  void gotoThisHdu() const;

  /**
   * @brief The parent file handler.
   * @warning
   * This is a reference to a pointer because we want the pointer to be valid
   * even if file is closed and reopened.
   */
  fitsfile *&m_fptr;

  /**
   * @brief The HDU index.
   */
  long m_index;
};

} // namespace FitsIO
} // namespace Euclid

#include "impl/RecordHdu.hpp"

#endif
