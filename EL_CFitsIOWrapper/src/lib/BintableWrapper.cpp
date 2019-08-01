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

#include "EL_CFitsIOWrapper/BintableWrapper.h"
#include "EL_CFitsIOWrapper/CfitsioUtils.h"

namespace Cfitsio {
namespace Bintable {

std::size_t column_index(fitsfile* fptr, std::string name) {
	int index;
	int status = 0;
	fits_get_colnum(fptr, CASESEN, to_char_ptr(name), &index, &status);
	may_throw_cfitsio_error(status);
	return index;
}

struct std::vector<std::string> internal::ColumnReader<std::string>::read(fitsfile* fptr, std::string name) {
	auto char_ptr_data = internal::ColumnReader<std::vector<char>>::read(fptr, name);
	const std::size_t size = char_ptr_data.size();
	std::vector<std::string> data(size);
	for(std::size_t i=0; i<size; ++i)
		data[i] = std::string(char_ptr_data[i].data());
	return data;
}

}
}

