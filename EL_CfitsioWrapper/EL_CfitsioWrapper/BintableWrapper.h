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


///////////////
// INTERNAL //
/////////////


/// @cond INTERNAL
namespace internal {

/**
 * Helper structure to dispatch column IOs depending on the cell type
 * (scalar, string or vector + pointer for internal implementation).
 */
template<typename T>
struct ColumnDispatcher {
  static FitsIO::VecColumn<T> read(fitsfile* fptr, std::string name);
  static void write(fitsfile* fptr, const FitsIO::Column<T>& column);
};

template<>
struct ColumnDispatcher<std::string> {
  static FitsIO::VecColumn<std::string> read(fitsfile* fptr, std::string name);
  static void write(fitsfile* fptr, const FitsIO::Column<std::string>& column);
};

template<typename T>
struct ColumnDispatcher<T*> {
  static FitsIO::VecColumn<T*> read(fitsfile* fptr, std::string name);
  static void write(fitsfile* fptr, const FitsIO::Column<T*>& column);
};

template<typename T>
struct ColumnDispatcher<std::vector<T>> {
  static FitsIO::VecColumn<std::vector<T>> read(fitsfile* fptr, std::string name);
  static void write(fitsfile* fptr, const FitsIO::Column<std::vector<T>>& column);
};

template<typename T>
FitsIO::VecColumn<T> ColumnDispatcher<T>::read(fitsfile* fptr, std::string name) {
  size_t index = column_index(fptr, name);
  long rows;
  int status = 0;
  fits_get_num_rows(fptr, &rows, &status);
  may_throw_cfitsio_error(status);
  FitsIO::VecColumn<T> column({name, "", 1}, std::vector<T>(rows));
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
  may_throw_cfitsio_error(status);
  return column;
}

template<typename T>
FitsIO::VecColumn<T*> ColumnDispatcher<T*>::read(fitsfile* fptr, std::string name) {
  size_t index = column_index(fptr, name);
  long rows;
  int status = 0;
  fits_get_num_rows(fptr, &rows, &status); //TODO wrap
  may_throw_cfitsio_error(status);
  long repeat;
  fits_get_coltype(fptr, index, nullptr, &repeat, nullptr, &status); //TODO wrap
  FitsIO::VecColumn<T*> column({name, "TODO", repeat}, std::vector<T*>(rows)); //TODO unit
  for(long i=0; i<rows; ++i)
    column.vector()[i] = (T*) malloc(repeat * sizeof(T));
  fits_read_col(
    fptr,
    TypeCode<T*>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelemn (1-based)
    column.nelements(), // nelements
    nullptr, // nulval
    column.data(),
    nullptr, // anynul
    &status
  );
  may_throw_cfitsio_error(status);
  return column;
}

template<typename T>
FitsIO::VecColumn<std::vector<T>> ColumnDispatcher<std::vector<T>>::read(fitsfile* fptr, std::string name) {
  const auto ptr_col = ColumnDispatcher<T*>::read(fptr, name);
  const auto rows = ptr_col.rows();
  FitsIO::VecColumn<std::vector<T>> column(
      {ptr_col.info.name, ptr_col.info.unit, ptr_col.info.repeat},
      std::vector<std::vector<T>>(rows));
  for(std::size_t i=0; i<rows; ++i) {
    T* ptr_i = ptr_col.vector()[i];
    column.vector()[i].assign(ptr_i, ptr_i + ptr_col.info.repeat);
  }
  return column;
}

template<typename T>
void ColumnDispatcher<T>::write(fitsfile* fptr, const FitsIO::Column<T>& column) {
  size_t index = column_index(fptr, column.info.name);
  const auto begin = column.data();
  const auto end = begin + column.rows();
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
  may_throw_cfitsio_error(status);
}

template<typename T>
void ColumnDispatcher<T*>::write(fitsfile* fptr, const FitsIO::Column<T*>& column) {
  size_t index = column_index(fptr, column.info.name);
  const auto begin = column.data();
  const auto end = begin + column.rows();
  std::vector<T*> nonconst_data(begin, end); // We need a non-const data for CFitsIO
  //TODO avoid copy
  int status = 0;
  fits_write_col(
    fptr,
    TypeCode<T*>::for_bintable(), // datatype
    index, // colnum
    1, // firstrow (1-based)
    1, // firstelem (1-based)
    column.nelements(), // nelements
    nonconst_data.data(),
    &status
    );
  may_throw_cfitsio_error(status);
}

template<typename T>
void ColumnDispatcher<std::vector<T>>::write(fitsfile* fptr, const FitsIO::Column<std::vector<T>>& column) {
  const auto rows = column.rows();
  FitsIO::VecColumn<T*> ptr_column({column.info.name, column.info.unit, column.info.repeat}, std::vector<T*>(rows));
  for(std::size_t i=0; i<rows; ++i) { //TODO transform
    const auto& data_i = *(column.data() + i); //TODO check
    ptr_column.vector()[i] = (T*) malloc(column.info.repeat * sizeof(T));
    std::copy(data_i.data(), data_i.data() + data_i.size(), ptr_column.vector()[i]);
  }
  ColumnDispatcher<T*>::write(fptr, ptr_column);
  for(std::size_t i=0; i<rows; ++i)
    free(ptr_column.vector()[i]); //TODO check
}

}
/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
FitsIO::VecColumn<T> read_column(fitsfile* fptr, std::string name) {
  return internal::ColumnDispatcher<T>::read(fptr, name);
}

template<typename T>
void write_column(fitsfile* fptr, const FitsIO::Column<T>& column) {
    internal::ColumnDispatcher<T>::write(fptr, column);
}


}
}
}

#endif
