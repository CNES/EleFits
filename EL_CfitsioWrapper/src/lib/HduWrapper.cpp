/**
 * @file src/lib/HduHandler.cpp
 * @date 07/23/19
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

#include "EL_CfitsioWrapper/ErrorWrapper.h"
#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/RecordWrapper.h"

namespace Cfitsio {
namespace HDU {

std::size_t count(fitsfile* fptr) {
	int count;
	int status = 0;
	fits_get_num_hdus(fptr, &count, &status);
	may_throw_cfitsio_error(status);
	return count;
}

std::size_t current_index(fitsfile* fptr) {
	int index;
	fits_get_hdu_num(fptr, &index);
	return index;
}

std::string current_name(fitsfile* fptr) {
	try {
		return Record::read_value(fptr, "EXTNAME");
	} catch(const std::exception& e) {
		// EXTNAME not provided
		return "";
	}
}

Type current_type(fitsfile* fptr) {
	int type = 0;
	int status = 0;
	fits_get_hdu_type(fptr, &type, &status);
	if(type == BINARY_TBL)
		return Type::BINTABLE;
	if(current_has_data(fptr))
		return Type::IMAGE;
	return Type::METADATA;
}

bool current_is_primary(fitsfile* fptr) {
	return current_index(fptr) == 1;
}

bool current_has_data(fitsfile* fptr) {
	return true; //TODO
}

bool goto_index(fitsfile* fptr, std::size_t index) {
	if(index == current_index(fptr))
		return false;
	int type = 0;
	int status = 0;
	fits_movabs_hdu(fptr, index, &type, &status);
	may_throw_cfitsio_error(status);
	return true;
}

bool goto_name(fitsfile* fptr, std::string name) {
	if(name == "")
		return false;
	if(name == "Primary")
	  return goto_primary(fptr);
	if(name == current_name(fptr))
		return false;
	int status = 0;
	fits_movnam_hdu(fptr, ANY_HDU, to_char_ptr(name).get(), 0, &status);
	may_throw_cfitsio_error(status);
	return true;
}

bool goto_next(fitsfile* fptr, std::size_t step) {
	if(step == 0)
		return false;
	int status = 0;
	int type = 0;
	fits_movrel_hdu(fptr, step, &type, &status);
	may_throw_cfitsio_error(status);
	return true;
}

bool goto_primary(fitsfile* fptr) {
	return goto_index(fptr, 1);
}

bool init_primary(fitsfile* fptr) {
	if(count(fptr) > 0)
		return false;
	create_image_extension(fptr, "", Image::Raster<char, 1>());
	return true;
}

bool update_name(fitsfile* fptr, std::string name) {
	if(name == "")
		return false;
	Record::update_record<std::string>(fptr, "EXTNAME", name);
	return true;
}

void create_metadata_extension(fitsfile* fptr, std::string name) {
	may_throw_readonly_error(fptr);
	const int default_type = BYTE_IMG;
	long naxes = 0;
	int status = 0;
	fits_create_img(fptr, default_type, naxes, nullptr, &status);
	may_throw_cfitsio_error(status);
	update_name(fptr, name);
}

}
}

