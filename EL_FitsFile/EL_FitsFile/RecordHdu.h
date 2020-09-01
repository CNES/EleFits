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
  RecordHdu(fitsfile*& file, long index);

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
  void rename(const std::string& name) const;

  /**
   * @brief List the record keywords.
   */
  std::vector<std::string> keywords() const;

  /**
   * @brief Parse a record.
   */
  template<typename T>
  Record<T> parseRecord(const std::string& keyword) const;

  /**
   * @brief Parse several records.
   */
  template<typename... Ts>
  std::tuple<Record<Ts>...> parseRecords(const std::vector<std::string>& keywords) const;

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
  template<class Return, typename... Ts>
  Return parseRecordsAs(const std::vector<std::string>& keywords) const;
  
  /**
   * @brief Write a record.
   */
  template<typename T>
  void writeRecord(const Record<T>& record) const;

  /**
   * @brief Write a record.
   */
  template<typename T>
  void writeRecord(const std::string& keyword, T value, const std::string& unit="", const std::string& comment="") const;

  /**
   * @brief Write several records.
   */
  template<typename... Ts>
  void writeRecords(const Record<Ts>&... records) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   */
  template<typename T>
  void updateRecord(const Record<T>& record) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   */
  template<typename T>
  void updateRecord(const std::string& keyword, T value, const std::string& unit="", const std::string& comment="") const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   */
  template<typename... Ts>
  void updateRecords(const Record<Ts>&... records) const;

  /**
   * @brief Delete a record.
   */
  void deleteRecord(const std::string& keyword) const;

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
  fitsfile*& m_fptr;

  /**
   * @brief The HDU index.
   */
  long m_index;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Record<T> RecordHdu::parseRecord(const std::string& keyword) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecord<T>(m_fptr, keyword);
}

template<typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parseRecords(const std::vector<std::string>& keywords) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecords<Ts...>(m_fptr, keywords);
}

template<class Return, typename... Ts>
Return RecordHdu::parseRecordsAs(const std::vector<std::string>& keywords) const {
  gotoThisHdu();
  return Cfitsio::Header::parseRecordsAs<Return, Ts...>(m_fptr, keywords);
}

template<typename T>
void RecordHdu::writeRecord(const Record<T>& record) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecord(m_fptr, record);
}

template<typename T>
void RecordHdu::writeRecord(const std::string& keyword, T value, const std::string& unit, const std::string& comment) const {
  writeRecord(Record<T>(keyword, value, unit, comment));
}

template<typename... Ts>
void RecordHdu::writeRecords(const Record<Ts>&... records) const {
  gotoThisHdu();
  Cfitsio::Header::writeRecords(m_fptr, records...);
}

template<typename T>
void RecordHdu::updateRecord(const Record<T>& record) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecord(m_fptr, record);
}

template<typename T>
void RecordHdu::updateRecord(const std::string& keyword, T value, const std::string& unit, const std::string& comment) const {
  updateRecord(Record<T>(keyword, value, unit, comment));
}

template<typename... Ts>
void RecordHdu::updateRecords(const Record<Ts>&... records) const {
  gotoThisHdu();
  Cfitsio::Header::updateRecords(m_fptr, records...);
}

#ifndef DECLARE_PARSE_RECORD
#define DECLARE_PARSE_RECORD(T) \
  extern template Record<T> RecordHdu::parseRecord(const std::string&) const;
DECLARE_PARSE_RECORD(char)
DECLARE_PARSE_RECORD(short)
DECLARE_PARSE_RECORD(int)
DECLARE_PARSE_RECORD(long)
DECLARE_PARSE_RECORD(float)
DECLARE_PARSE_RECORD(double)
DECLARE_PARSE_RECORD(unsigned char)
DECLARE_PARSE_RECORD(unsigned short)
DECLARE_PARSE_RECORD(unsigned int)
DECLARE_PARSE_RECORD(unsigned long)
#undef DECLARE_PARSE_RECORD
#endif

#ifndef DECLARE_WRITE_RECORD
#define DECLARE_WRITE_RECORD(T) \
  extern template void RecordHdu::writeRecord(const Record<T>&) const;
DECLARE_WRITE_RECORD(char)
DECLARE_WRITE_RECORD(short)
DECLARE_WRITE_RECORD(int)
DECLARE_WRITE_RECORD(long)
DECLARE_WRITE_RECORD(float)
DECLARE_WRITE_RECORD(double)
DECLARE_WRITE_RECORD(unsigned char)
DECLARE_WRITE_RECORD(unsigned short)
DECLARE_WRITE_RECORD(unsigned int)
DECLARE_WRITE_RECORD(unsigned long)
#undef DECLARE_WRITE_RECORD
#endif

}
}

#endif
