/**
 * @file EL_FitsData/Column.h
 * @date 10/21/19
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

#ifndef _EL_FITSDATA_COLUMN_H
#define _EL_FITSDATA_COLUMN_H


#include <string>
#include <tuple>
#include <vector>


namespace Euclid {
namespace FitsIO {

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

}

/// @endcond


/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
std::size_t Column<T>::nelements() const {
	return internal::column_nelements(*this);
}


}
}

#endif
