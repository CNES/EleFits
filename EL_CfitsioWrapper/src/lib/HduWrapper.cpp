/**
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
#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Hdu {

long count(fitsfile* fptr) {
  int count = 0;
  int status = 0;
  fits_get_num_hdus(fptr, &count, &status);
  mayThrowCfitsioError(status);
  return count;
}

long current_index(fitsfile* fptr) {
  int index = 0;
  fits_get_hdu_num(fptr, &index);
  return index;
}

std::string current_name(fitsfile* fptr) {
  try {
    return Header::parse_record<std::string>(fptr, "EXTNAME");
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
  return Type::IMAGE;
}

bool current_is_primary(fitsfile* fptr) {
  return current_index(fptr) == 1;
}

bool goto_index(fitsfile* fptr, long index) {
  if(index == current_index(fptr))
    return false;
  int type = 0;
  int status = 0;
  fits_movabs_hdu(fptr, static_cast<int>(index), &type, &status); // HDU indices are int
  mayThrowCfitsioError(status, "Cannot access HDU " + std::to_string(index));
  return true;
}

bool goto_name(fitsfile* fptr, const std::string& name) {
  if(name == "")
    return false;
  if(name == "Primary")
    return goto_primary(fptr);
  if(name == current_name(fptr))
    return false;
  int status = 0;
  fits_movnam_hdu(fptr, ANY_HDU, toCharPtr(name).get(), 0, &status);
  mayThrowCfitsioError(status);
  return true;
}

bool goto_next(fitsfile* fptr, long step) {
  if(step == 0)
    return false;
  int status = 0;
  int type = 0;
  fits_movrel_hdu(fptr, static_cast<int>(step), &type, &status); // HDU indices are int
  mayThrowCfitsioError(status);
  return true;
}

bool goto_primary(fitsfile* fptr) {
  return goto_index(fptr, 1);
}

bool init_primary(fitsfile* fptr) {
  if(count(fptr) > 0)
    return false;
  create_metadata_extension(fptr, "");
  return true;
}

bool update_name(fitsfile* fptr, const std::string& name) {
  if(name == "")
    return false;
  Header::update_record(fptr, FitsIO::Record<std::string>("EXTNAME", name));
  return true;
}

void create_metadata_extension(fitsfile* fptr, const std::string& name) {
  create_image_extension<unsigned char, 0>(fptr, name, FitsIO::pos_type<0>());
}

void delete_hdu(fitsfile *fptr, long index) {
  goto_index(fptr, index);
  int status = 0;
  fits_delete_hdu(fptr, nullptr, &status);
  mayThrowCfitsioError(status, "Cannot delete HDU " + std::to_string(index));
}

}
}
}
