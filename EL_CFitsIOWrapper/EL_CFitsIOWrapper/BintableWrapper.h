/**
 * @file EL_CFitsIOWrapper/BintableWrapper.h
 * @date 07/27/19
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

#ifndef _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H
#define _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H

#include <tuple>
#include <vector>

#include "EL_CFitsIOWrapper/TypeWrapper.h"

namespace Cfitsio {

/**
 * @brief Bintable-related functions.
 */
namespace Bintable {

/**
 * @brief Type for a column info, i.e. { name, width, unit }
 */
template<typename T>
using column_info = std::tuple<std::string, std::size_t, std::string>;

/**
 * @brief Get the index of a Bintable column.
 */
std::size_t column_index(fitsfile *fptr, std::string name);

/**
 * @brief Read a Bintable column with given name.
 */
template<typename T>
std::vector<T> read_column(fitsfile *fptr, std::string name);

/**
 * @brief Write a binary table column with given name.
 */
template<typename T>
void write_column(fitsfile *fptr, const column_info<T>& info, const std::vector<T>& column);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
std::vector<T> read_column(fitsfile* fptr, std::string name) {
  size_t index = column_index(fptr, name);
  long rows;
  int status = 0;
  fits_get_num_rows(fptr, &rows, &status);
  may_throw_cfitsio_error(status);
  std::vector<T> data(rows);
  fits_read_col(
      fptr,
      TypeCode<T>::for_table(), // datatype
      index, // colnum
      1, // firstrow (1-based)
      1, // firstelemn (1-based)
      rows, // nelements
      nullptr, // nulval
      data.data(),
      nullptr, // anynul
      &status
      );
  may_throw_cfitsio_error(status);
  return data;
}

template<typename T>
void write_column(fitsfile* fptr, const column_info<T>& info, const std::vector<T>& data) {
    size_t index = column_index(fptr, std::get<0>(info));
    std::vector<T> nonconst_data = data; // We need a non-const data for CFitsIO
    //TODO avoid copy
    int status = 0;
    fits_write_col(
          fptr,
          TypeCode<T>::for_table(), // datatype
          index, // colnum
          1, // firstrow (1-based)
          1, // firstelem (1-based)
          std::get<1>(info), // nelements
          &data[0],
          &status
          );
    may_throw_cfitsio_error(status);
}


}
}

#endif

