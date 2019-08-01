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

template<typename T>
struct Column {

	std::string name;
	std::size_t width;
	std::string unit;
	std::vector<T> data;

};

/**
 * @brief Get the index of a Bintable column.
 */
std::size_t column_index(fitsfile* fptr, std::string name);

/**
 * @brief Read a Bintable column with given name.
 */
template<typename T>
std::vector<T> read_column(fitsfile* fptr, std::string name);

/**
 * @brief Write a binary table column with given name.
 */
template<typename T>
void write_column(fitsfile* fptr, const Column<T>& column);


///////////////
// INTERNAL //
/////////////


namespace internal {

template<typename T>
struct ColumnReader {
    static std::vector<T> read(fitsfile* fptr, std::string name) {
		size_t index = column_index(fptr, name);
		long rows;
		int status = 0;
		fits_get_num_rows(fptr, &rows, &status);
		may_throw_cfitsio_error(status);
		std::vector<T> data(rows);
		fits_read_col(
			fptr,
			TypeCode<T>::for_bintable(), // datatype
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
};

template<>
struct ColumnReader<std::string> {
	static std::vector<std::string> read(fitsfile* fptr, std::string name);
};

template<typename T>
struct ColumnReader<std::vector<T>> {
    static std::vector<std::vector<T>> read(fitsfile* fptr, std::string name) {
		size_t index = column_index(fptr, name);
		long rows;
		int status = 0;
		fits_get_num_rows(fptr, &rows, &status);
		may_throw_cfitsio_error(status);
		long repeat;
		fits_get_coltype(fptr, index, nullptr, &repeat, nullptr, &status);
		T** ptr_data = new T*[rows];
		for(std::size_t i=0; i<rows; ++i)
			ptr_data[i] = new T[repeat];
		fits_read_col(
			fptr,
			TypeCode<std::string>::for_bintable(), // datatype
			index, // colnum
			1, // firstrow (1-based)
			1, // firstelemn (1-based)
			rows, // nelements
			nullptr, // nulval
			ptr_data,
			nullptr, // anynul
			&status
		);
		may_throw_cfitsio_error(status);
		std::vector<std::vector<T>> data(rows);
		for(std::size_t i=0; i<rows; ++i) {
			T* ptr_i = ptr_data[i];
			data[i] = std::vector<T>(ptr_i, ptr_i + repeat);
			delete[] ptr_data[i];
		}
		delete[] ptr_data;
		return data;
	}
};

}

/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
std::vector<T> read_column(fitsfile* fptr, std::string name) {
	return internal::ColumnReader<T>::read(fptr, name);
}

template<typename T>
void write_column(fitsfile* fptr, const Column<T>& column) {
    size_t index = column_index(fptr, column.name);
    std::vector<T> nonconst_data = column.data; // We need a non-const data for CFitsIO
    //TODO avoid copy
    int status = 0;
    fits_write_col(
		fptr,
		TypeCode<T>::for_bintable(), // datatype
		index, // colnum
		1, // firstrow (1-based)
		1, // firstelem (1-based)
		column.data.size(), // nelements
		nonconst_data.data(),
		&status
		);
    may_throw_cfitsio_error(status);
}


}
}

#endif

