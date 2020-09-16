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

namespace Euclid {
namespace FitsIO {

/**
 * @brief Header reader-writer.
 * @details
 * When reading or writing several Records, it is recommended to use the plural form of the methods
 * (e.g. one call to RecordHdu::writeRecords instead of several calls to RecordHdu::writeRecord),
 * which are optimized.
 * @warning
 * There is a known bug in CFitsIO with the reading of Record<unsigned long>:
 * if the value is greater than max(long), CFitsIO returns an overflow error.
 */
class RecordHdu {

public:
  /**
   * @brief Constructor.
   * @warning
   * You should not instantiate RecordHdus yourself,
   * but using the dedicated MefFile creation method.
   */
  RecordHdu(fitsfile *&file, long index);

  /**
   * @brief Destructor.
   */
  virtual ~RecordHdu() = default;

  /**
   * @brief 1-based index of the HDU.
   */
  long index() const;

  /**
   * @brief Read the extension name.
   */
  std::string name() const;

  /**
   * @brief Write or update the extension name.
   */
  void rename(const std::string &name) const;

  /**
   * @brief List the record keywords.
   */
  std::vector<std::string> keywords() const;

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
   * @tparam Return A structure which can be constructed as:
   * \code Return { T1, T2, ... } \endcode
   * or:
   * \code Return { Record<T1>, Record<T2>, ... } \endcode
   * like a simple structure:
   * \code struct Return { T1 p1; T2 p2; ... }; \endcode
   * or a class with such constructor:
   * \code Return::Return(T1, T2, ...) \endcode
   * @details This is generally more convenient than a tuple
   * because you chose how to to access the records in your own class
   * insted of accessing them by their indices -- with \c std::get<i>(tuple).
   */
  template <class Return, typename... Ts>
  Return parseRecordsAs(const std::vector<std::string> &keywords) const;

  /**
   * @brief Parse several homogeneous records.
   * @tparam T The common value type.
   * @details
   * In addition to parsing homogeneous records, like a set of integer records,
   * this method can be used with boost::any records to allow for runtime type deduction:
   * @code
   * auto records = f.parseRecordVector<boost::any>({ "INT", "FLOAT", "DOUBLE", "BOOL" });
   * int value = records.as<int>("INT");
   * @endcode
   */
  template <typename T>
  RecordVector<T> parseRecordVector(const std::vector<std::string> &keywords) const;

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
   */
  template <typename T>
  void writeRecords(const std::vector<Record<T>> &records) const;

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
