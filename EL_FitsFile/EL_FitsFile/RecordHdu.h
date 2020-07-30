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
 */
class RecordHdu {

public:

  /**
   * @brief Constructor.
   * @warning
   * You should not instantiate RecordHdus yourself,
   * but using the dedicated MefFile creation method.
   */
  RecordHdu(fitsfile*& file, std::size_t index);

  /**
   * @brief Destructor.
   */
  virtual ~RecordHdu() = default;

  /**
   * @brief 1-based index of the HDU.
   */
  std::size_t index() const;

  /**
   * @brief Read the extension name.
   */
  std::string name() const;

  /**
   * @brief Write or update the extension name.
   */
  void rename(std::string) const;

  /**
   * @brief List the record keywords.
   */
  std::vector<std::string> keywords() const;

  /**
   * @brief Parse a record.
   */
  template<typename T>
  Record<T> parse_record(std::string keyword) const;

  /**
   * @brief Parse several records.
   */
  template<typename... Ts>
  std::tuple<Record<Ts>...> parse_records(const std::vector<std::string>& keywords) const;

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
  Return parse_records_as(const std::vector<std::string>& keywords) const;
  
  /**
   * @brief Write a record.
   */
  template<typename T>
  void write_record(const Record<T>& record) const;

  /**
   * @brief Write a record.
   */
  template<typename T>
  void write_record(std::string keyword, T value, std::string unit="", std::string comment="") const;

  /**
   * @brief Write several records.
   */
  template<typename... Ts>
  void write_records(const Record<Ts>&... records) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   */
  template<typename T>
  void update_record(const Record<T>& record) const;

  /**
   * @brief Update a record if it exists; write a new record otherwise.
   */
  template<typename T>
  void update_record(std::string keyword, T value, std::string unit="", std::string comment="") const;

  /**
   * @brief Update several records if they exist; write new records otherwise.
   */
  template<typename... Ts>
  void update_records(const Record<Ts>&... records) const;

  /**
   * @brief Delete a record.
   */
  void delete_record(std::string keyword) const;

protected:

  void goto_this_hdu() const;

  /**
   * This is a reference to a pointer because we want the pointer to be valid
   * even if file is closed and reopened.
   */
  fitsfile*& m_fptr;

  std::size_t m_index;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Record<T> RecordHdu::parse_record(std::string keyword) const {
  goto_this_hdu();
  return Cfitsio::Header::parse_record<T>(m_fptr, keyword);
}

template<typename... Ts>
std::tuple<Record<Ts>...> RecordHdu::parse_records(const std::vector<std::string>& keywords) const {
  goto_this_hdu();
  return Cfitsio::Header::parse_records<Ts...>(m_fptr, keywords);
}

template<class Return, typename... Ts>
Return RecordHdu::parse_records_as(const std::vector<std::string>& keywords) const {
  goto_this_hdu();
  return Cfitsio::Header::parse_records_as<Return, Ts...>(m_fptr, keywords);
}

template<typename T>
void RecordHdu::write_record(const Record<T>& record) const {
  goto_this_hdu();
  Cfitsio::Header::write_record(m_fptr, record);
}

template<typename T>
void RecordHdu::write_record(std::string keyword, T value, std::string unit, std::string comment) const {
  write_record(Record<T>(keyword, value, unit, comment));
}

template<typename... Ts>
void RecordHdu::write_records(const Record<Ts>&... records) const {
  goto_this_hdu();
  Cfitsio::Header::write_records(m_fptr, records...);
}

template<typename T>
void RecordHdu::update_record(const Record<T>& record) const {
  goto_this_hdu();
  Cfitsio::Header::update_record(m_fptr, record);
}

template<typename T>
void RecordHdu::update_record(std::string keyword, T value, std::string unit, std::string comment) const {
  update_record(Record<T>(keyword, value, unit, comment));
}

template<typename... Ts>
void RecordHdu::update_records(const Record<Ts>&... records) const {
  goto_this_hdu();
  Cfitsio::Header::update_records(m_fptr, records...);
}

#ifndef DECLARE_PARSE_RECORD
#define DECLARE_PARSE_RECORD(T) \
  extern template Record<T> RecordHdu::parse_record(std::string) const;
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
  extern template void RecordHdu::write_record(const Record<T>&) const;
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
