/**
 * @file EL_FitsFile/BintableHdu.h
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

#ifndef _EL_FITSFILE_BINTABLEHDU_H
#define _EL_FITSFILE_BINTABLEHDU_H

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {


/**
 * @brief Bintable HDU reader-writer.
 */
class BintableHdu : public RecordHdu {

public:

  /**
   * @brief Constructor.
   * @warning
   * You should not instantiate RecordHdus yourself,
   * but using the dedicated MefFile creation method.
   */
  BintableHdu(fitsfile*& fptr, std::size_t index);

  /**
   * @brief Destructor.
   */
  virtual ~BintableHdu() = default;

  /**
   * @brief Read a column with given name.
   */
  template<typename T>
  VecColumn<T> read_column(std::string name) const;

  /**
   * @brief Read several columns with given names.
   */
  template<typename... Ts>
  std::tuple<VecColumn<Ts>...> read_columns(std::vector<std::string> names) const;

  /**
   * @brief Write a column.
   */
  template<typename T>
  void write_column(const Column<T>& column) const;

  /**
   * @brief Write several columns.
   */
  template<typename... Ts>
  void write_columns(const Column<Ts>&... columns) const;

  /**
   * @brief Append a column.
   */
  template<typename T>
  void append_column(const Column<T>& column) const;

  /**
   * @brief Append several columns.
   */
  template<typename... Ts>
  void append_columns(const Column<Ts>&... columns) const;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
VecColumn<T> BintableHdu::read_column(std::string name) const {
  goto_this_hdu();
  return Cfitsio::Bintable::read_column<T>(m_fptr, name);
}

template<typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::read_columns(std::vector<std::string> names) const {
  goto_this_hdu();
  return Cfitsio::Bintable::read_columns<Ts...>(m_fptr, names);
}

template<typename T>
void BintableHdu::write_column(const Column<T>& column) const {
  goto_this_hdu();
  Cfitsio::Bintable::write_column(m_fptr, column);
}

template<typename... Ts>
void BintableHdu::write_columns(const Column<Ts>&... columns) const {
  goto_this_hdu();
  Cfitsio::Bintable::write_columns(m_fptr, columns...);
}

template<typename T>
void BintableHdu::append_column(const Column<T>& column) const {
  goto_this_hdu();
  Cfitsio::Bintable::append_column(m_fptr, column);
}

template<typename... Ts>
void BintableHdu::append_columns(const Column<Ts>&... columns) const {
  goto_this_hdu();
  Cfitsio::Bintable::append_columns(m_fptr, columns...);
}

}
}

#endif
