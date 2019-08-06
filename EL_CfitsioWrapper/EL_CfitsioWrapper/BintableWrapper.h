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

#include "EL_CfitsioWrapper/TypeWrapper.h"

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

/**
 * @brief Bintable column data and metadata.
 */
template<typename T>
struct Column {

	std::string name;

	/**
	 * @brief Repeat count of the column, i.e. number of values per cell.
	 * @warning CFitsIO uses long instead of size_t
	 */
	long repeat;

	std::string unit;

	std::vector<T> data;

	/**
	 * @brief Number of elements in the column, i.e. number of rows * repeat count.
	 * @warning For strings, CFitsIO requires nelements to be just the number of rows.
	 */
	std::size_t nelements() const; //TODO long?

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


/// @cond INTERNAL
namespace internal {

/**
 * Helper function to specialize nelements for strings.
 */
template<typename T>
std::size_t column_nelements(const Column<T>& column);

/**
 * In general, nelements is the number of values,
 * i.e. number of rows * repeat count.
 */
template<typename T>
inline std::size_t column_nelements(const Column<T>& column) {
	return column.repeat * column.data.size();
}

/**
 * For strings, nelements is the number of strings,
 * not the number of characters.
 */
template<>
inline std::size_t column_nelements(const Column<char*>& column) {
	return column.data.size();
}

/**
 * Helper structure to dispatch column IOs depending on the cell type
 * (scalar, string or vector + pointer for internal implementation).
 */
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
	for(std::size_t i=0; i<rows; ++i)
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
	const auto ptr_col = ColumnDispatcher<T*>::read(fptr, name);
	const auto rows = ptr_col.data.size();
	Column<std::vector<T>> column { ptr_col.name, ptr_col.repeat, "TODO", std::vector<std::vector<T>>(rows) }; //TODO unit
	for(std::size_t i=0; i<rows; ++i) {
		T* ptr_i = ptr_col.data[i];
		column.data[i].assign(ptr_i, ptr_i + ptr_col.repeat);
		// delete[] ptr_i; //TODO keep?
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
	// for(T* dptr : ptr_column.data)
	// 	delete[] dptr; //TODO keep?
}

}
/// @endcond


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

