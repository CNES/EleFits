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

#ifndef _EL_FITSFILE_BINTABLEHDU_H
#define _EL_FITSFILE_BINTABLEHDU_H

#include <string>

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
  BintableHdu(fitsfile*& fptr, long index);

  /**
   * @brief Destructor.
   */
  virtual ~BintableHdu() = default;

  /**
   * @brief Read a column with given name.
   */
  template<typename T>
  VecColumn<T> read_column(const std::string& name) const;

  /**
   * @brief Read several columns with given names.
   */
  template<typename... Ts>
  std::tuple<VecColumn<Ts>...> read_columns(const std::vector<std::string>& names) const;

  /**
   * @brief Write a column.
   */
  template<typename T>
  void write_column(const Column<T>& column) const;

  /**
   * @brief Write several columns.
   * @warning All columns should have the same number of rows.
   */
  template<typename... Ts>
  void write_columns(const Column<Ts>&... columns) const;

  /**
   * @brief Append a column.
   * @warning The column should have the same number of rows as the existing columns.
   */
  template<typename T>
  void append_column(const Column<T>& column) const;

  /**
   * @brief Append several columns.
   * @warning All new columns should have the same number of rows as the existing columns.
   */
  template<typename... Ts>
  void append_columns(const Column<Ts>&... columns) const;

};


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
VecColumn<T> BintableHdu::read_column(const std::string& name) const {
  goto_this_hdu();
  return Cfitsio::Bintable::read_column<T>(m_fptr, name);
}

template<typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::read_columns(const std::vector<std::string>& names) const {
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

#ifndef DECLARE_READ_COLUMN
#define DECLARE_READ_COLUMN(T) \
    extern template VecColumn<T> BintableHdu::read_column(const std::string&) const;
DECLARE_READ_COLUMN(char)
DECLARE_READ_COLUMN(short)
DECLARE_READ_COLUMN(int)
DECLARE_READ_COLUMN(long)
DECLARE_READ_COLUMN(float)
DECLARE_READ_COLUMN(double)
DECLARE_READ_COLUMN(unsigned char)
DECLARE_READ_COLUMN(unsigned short)
DECLARE_READ_COLUMN(unsigned int)
DECLARE_READ_COLUMN(unsigned long)
#undef DECLARE_READ_COLUMN
#endif

#ifndef DECLARE_WRITE_COLUMN
#define DECLARE_WRITE_COLUMN(T) \
    extern template void BintableHdu::write_column(const Column<T>&) const;
DECLARE_WRITE_COLUMN(char)
DECLARE_WRITE_COLUMN(short)
DECLARE_WRITE_COLUMN(int)
DECLARE_WRITE_COLUMN(long)
DECLARE_WRITE_COLUMN(float)
DECLARE_WRITE_COLUMN(double)
DECLARE_WRITE_COLUMN(unsigned char)
DECLARE_WRITE_COLUMN(unsigned short)
DECLARE_WRITE_COLUMN(unsigned int)
DECLARE_WRITE_COLUMN(unsigned long)
#undef DECLARE_WRITE_COLUMN
#endif

}
}


#endif
