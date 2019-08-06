/**
 * @file src/lib/BintableWrapper.cpp
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

#include <algorithm>

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"

namespace Cfitsio {
namespace Bintable {

std::size_t column_index(fitsfile* fptr, std::string name) {
	int index;
	int status = 0;
	fits_get_colnum(fptr, CASESEN, to_char_ptr(name).get(), &index, &status);
	may_throw_cfitsio_error(status);
	return index;
}

Column<std::string> internal::ColumnDispatcher<std::string>::read(fitsfile* fptr, std::string name) {
	auto ptr_col = ColumnDispatcher<char*>::read(fptr, name);
	const auto rows = ptr_col.data.size();
	Column<std::string> column { ptr_col.name, ptr_col.repeat, ptr_col.unit, std::vector<std::string>(rows) };
	for(std::size_t i=0; i<rows; ++i) {
		char* ptr_i = ptr_col.data[i];
		column.data[i] = std::string(ptr_i);
	}
	return column;
}

void internal::ColumnDispatcher<std::string>::write(fitsfile* fptr, const Column<std::string>& column) {
	c_str_array array(column.data);
	Column<char*> char_ptr_column { column.name, column.repeat, column.unit, std::move(array.c_str_vector) };
	internal::ColumnDispatcher<char*>::write(fptr, char_ptr_column);
}

}
}

