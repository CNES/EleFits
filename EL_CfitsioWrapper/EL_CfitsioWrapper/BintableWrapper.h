/**
 * @file EL_CfitsioWrapper/BintableWrapper.h
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

#include "EL_FitsData/Column.h"

#include "EL_CfitsioWrapper/TypeWrapper.h"


namespace Euclid {
namespace Cfitsio {

/**
 * @brief Bintable-related functions.
 */
namespace Bintable {

/**
 * @brief Get the index of a Bintable column.
 */
std::size_t column_index(fitsfile* fptr, std::string name);

/**
 * @brief Read a Bintable column with given name.
 */
template<typename T>
FitsIO::VecColumn<T> read_column(fitsfile* fptr, std::string name);

/**
 * @brief Write a binary table column with given name.
 */
template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column);


/////////////////////
// IMPLEMENTATION //
///////////////////


template<>
FitsIO::VecColumn<std::string> read_column<std::string>(fitsfile* fptr, std::string name);

template<>
void write_column<std::string>(fitsfile* fptr, const FitsIO::Column<std::string>& column);

template<typename T>
FitsIO::VecColumn<T> read_column(fitsfile* fptr, std::string name) {
  size_t index = column_index(fptr, name);
  int typecode = 0;
  long repeat = 0;
  long width = 0;
  long rows = 0;
  int status = 0;
  fits_get_coltype(fptr, index, &typecode, &repeat, &width, &status);
  fits_get_num_rows(fptr, &rows, &status);
  may_throw_cfitsio_error(status, "Cannot read column dimensions");
  FitsIO::VecColumn<T> column({ name, "", repeat }, std::vector<T>(repeat * rows));
  fits_read_col(
    fptr,
    TypeCode<T>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelemn (1-based)
    column.nelements(), // nelements
    nullptr, // nulval
    column.data(),
    nullptr, // anynul
    &status
  );
  may_throw_cfitsio_error(status, "Cannot read column data");
  return column;
}

template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column) {
  size_t index = column_index(fptr, column.info.name);
  const auto begin = column.data();
  const auto end = begin + column.nelements();
  std::vector<T> nonconst_data(begin, end); // We need a non-const data for CFitsIO
  //TODO avoid copy
  int status = 0;
  fits_write_col(
    fptr,
    TypeCode<T>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelem (1-based)
    column.nelements(), // nelements
    nonconst_data.data(),
    &status
    );
  may_throw_cfitsio_error(status, "Cannot write column data");
}

}
}
}

#endif
