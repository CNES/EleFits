/**
 * @file EL_FitsFile/RecordHdu.h
 * @date 08/30/19
 * @author user
 *
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

  RecordHdu(fitsfile* file, std::size_t index);

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
   * @brief Write the extension name.
   */
  void rename(std::string) const;

  /**
   * @brief Parse a record.
   */
  template<typename T>
  Record<T> parse_record(std::string keyword) const;

  /**
   * @brief Parse records.
   */
  template<typename... Ts>
  std::tuple<Record<Ts>...> parse_records(const std::vector<std::string>& keywords) const;
  
  /**
   * @brief Write a record.
   */
  template<typename T>
  void write_record(const Record<T>& record) const;

  /**
   * @brief Write records.
   */
  template<typename T>
  void write_record(std::string keyword, T value, std::string unit="", std::string comment="") const;

  /**
   * @brief Write records.
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
   * @brief Update records if they exists; write new records otherwise.
   */
  template<typename... Ts>
  void update_records(const Record<Ts>&... records) const;

  /**
   * @brief Delete a record.
   */
  void delete_record(std::string keyword) const;

protected:

  void goto_this_hdu() const;

  fitsfile* m_fptr;

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

}
}

#endif
