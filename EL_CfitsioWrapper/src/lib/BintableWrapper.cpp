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

namespace Euclid {
namespace Cfitsio {
namespace Bintable {

std::size_t column_index(fitsfile* fptr, std::string name) {
	int index;
	int status = 0;
	fits_get_colnum(fptr, CASESEN, to_char_ptr(name).get(), &index, &status);
	may_throw_cfitsio_error(status);
	return index;
}

FitsIO::VecColumn<std::string> internal::ColumnDispatcher<std::string>::read(fitsfile* fptr, std::string name) {
	size_t index = column_index(fptr, name);
	long rows;
	int status = 0;
	fits_get_num_rows(fptr, &rows, &status);
	may_throw_cfitsio_error(status);
	long repeat;
	fits_get_coltype(fptr, index, nullptr, &repeat, nullptr, &status); //TODO wrap?
	may_throw_cfitsio_error(status);
	std::vector<char*> data(rows);
	for(long i=0; i < rows; ++i)
		data[i] = (char*) malloc(repeat);
	FitsIO::VecColumn<std::string> column({name, "TODO", repeat}, std::vector<std::string>(rows)); //TODO unit
	fits_read_col(
		fptr,
		TypeCode<std::string>::for_bintable(), // datatype
		index, // colnum
		1, // firstrow (1-based)
		1, // firstelemn (1-based)
		column.nelements(), // nelements
		nullptr, // nulval
		&data[0],
		nullptr, // anynul
		&status
	);
	may_throw_cfitsio_error(status);
	for(std::size_t i=0; i < static_cast<std::size_t>(rows); ++i) {
		column.vector()[i] = std::string(data[i]);
		free(data[i]);
	}
	return column;
}

void internal::ColumnDispatcher<std::string>::write(fitsfile* fptr, const FitsIO::Column<std::string>& column) {
	const auto begin = column.data();
	const auto end = begin + column.rows();
	c_str_array array(begin, end); //TODO avoid copy?
	std::size_t index = column_index(fptr, column.info.name);
	int status = 0;
	fits_write_col(
		fptr,
		TypeCode<std::string>::for_bintable(), // datatype
		index, // colnum
		1, // firstrow (1-based)
		1, // firstelem (1-based)
		column.nelements(), // nelements
		array.data(),
		&status
		);
	may_throw_cfitsio_error(status);
}

}
}
}
