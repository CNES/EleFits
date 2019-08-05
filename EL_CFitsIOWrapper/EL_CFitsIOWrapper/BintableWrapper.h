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
 * @brief Type for a column info, i.e. { name, repeat, unit }
 */
template<typename T>
using column_info = std::tuple<std::string, std::size_t, std::string>;

template<typename T>
struct Column {

	std::string name;
	std::size_t repeat;
	std::string unit;
	std::vector<T> data;

	std::size_t nelements() const;

};

/**
 * @brief Get the index of a Bintable column.
 */
std::size_t column_index(fitsfile* fptr, std::string name);

/**
 * @brief Read a Bintable column with given name.
 */
template<typename T>
Column<T> read_column(fitsfile* fptr, std::string name);

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
std::size_t column_nelements(const Column<T>& column);

template<typename T>
inline std::size_t column_nelements(const Column<T>& column) {
	return column.repeat * column.data.size();
}

template<>
inline std::size_t column_nelements(const Column<char*>& column) {
	return column.data.size();
}

template<typename T>
struct ColumnDispatcher {
	static Column<T> read(fitsfile* fptr, std::string name);
	static void write(fitsfile* fptr, const Column<T>& column);
};

template<typename T>
struct ColumnDispatcher<T*> {
	static Column<T*> read(fitsfile* fptr, std::string name);
	static void write(fitsfile* fptr, const Column<T*>& column);
};

template<>
struct ColumnDispatcher<std::string> {
	static Column<std::string> read(fitsfile* fptr, std::string name);
	static void write(fitsfile* fptr, const Column<std::string>& column);
};

template<typename T>
struct ColumnDispatcher<std::vector<T>> {
	static Column<std::vector<T>> read(fitsfile* fptr, std::string name);
	static void write(fitsfile* fptr, const Column<std::vector<T>>& column);
};

template<typename T>
Column<T> ColumnDispatcher<T>::read(fitsfile* fptr, std::string name) {
	size_t index = column_index(fptr, name);
	long rows;
	int status = 0;
	fits_get_num_rows(fptr, &rows, &status);
	may_throw_cfitsio_error(status);
	Column<T> column { name, 1, "", std::vector<T>(rows) };
	fits_read_col(
		fptr,
		TypeCode<T>::for_bintable(), // datatype
		index, // colnum
		1, // firstrow (1-based)
		1, // firstelemn (1-based)
		column.nelements(), // nelements
		nullptr, // nulval
		column.data.data(),
		nullptr, // anynul
		&status
	);
	may_throw_cfitsio_error(status);
	return column;
}

template<typename T>
Column<T*> ColumnDispatcher<T*>::read(fitsfile* fptr, std::string name) {
	size_t index = column_index(fptr, name);
	long rows;
	int status = 0;
	fits_get_num_rows(fptr, &rows, &status); //TODO wrap
	may_throw_cfitsio_error(status);
	long repeat;
	fits_get_coltype(fptr, index, nullptr, &repeat, nullptr, &status); //TODO wrap
	Column<T*> column { name, repeat, "TODO", std::vector<T*>(rows) }; //TODO unit
	for(long i=0; i<rows; ++i)
		column.data[i] = new T[repeat];
	fits_read_col(
		fptr,
		TypeCode<T*>::for_bintable(), // datatype
		index, // colnum
		1, // firstrow (1-based)
		1, // firstelemn (1-based)
		column.nelements(), // nelements
		nullptr, // nulval
		column.data.data(),
		nullptr, // anynul
		&status
	);
	may_throw_cfitsio_error(status);
	return column;
}

template<typename T>
Column<std::vector<T>> ColumnDispatcher<std::vector<T>>::read(fitsfile* fptr, std::string name) {
	auto index = column_index(fptr, name);
	int status = 0;
	may_throw_cfitsio_error(status);
	const auto ptr_col = ColumnDispatcher<T*>::read(fptr, name);
	const auto rows = ptr_col.data.size();
	Column<std::vector<T>> column { ptr_col.name, ptr_col.repeat, "TODO", std::vector<std::vector<T>>(rows) }; //TODO unit
	for(std::size_t i=0; i<rows; ++i) {
		const T* ptr_i = ptr_col.data[i];
		column.data[i].assign(ptr_i, ptr_i + ptr_col.repeat);
	}
	return column;
}

template<typename T>
void ColumnDispatcher<T>::write(fitsfile* fptr, const Column<T>& column) {
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
		column.nelements(), // nelements
		nonconst_data.data(),
		&status
		);
	may_throw_cfitsio_error(status);
}

template<typename T>
void ColumnDispatcher<T*>::write(fitsfile* fptr, const Column<T*>& column) {
	size_t index = column_index(fptr, column.name);
	std::vector<T*> nonconst_data = column.data; // We need a non-const data for CFitsIO
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
void ColumnDispatcher<std::vector<T>>::write(fitsfile* fptr, const Column<std::vector<T>>& column) {
	const auto rows = column.data.size();
	Column<T*> ptr_column { column.name, column.repeat, column.unit, std::vector<T*>(rows) };
	for(std::size_t i=0; i<rows; ++i) {
		const auto& data_i = column.data[i];
		ptr_column.data[i] = new T[data_i.size()];
		std::copy(data_i.data(), data_i.data() + data_i.size(), ptr_column.data[i]);
	}
	ColumnDispatcher<T*>::write(fptr, ptr_column);
}

}


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
std::size_t Column<T>::nelements() const {
	return internal::column_nelements(*this);
}

template<typename T>
Column<T> read_column(fitsfile* fptr, std::string name) {
	return internal::ColumnDispatcher<T>::read(fptr, name);
}

template<typename T>
void write_column(fitsfile* fptr, const Column<T>& column) {
    internal::ColumnDispatcher<T>::write(fptr, column);
}


}
}

#endif

