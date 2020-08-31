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

long currentIndex(fitsfile* fptr) {
  int index = 0;
  fits_get_hdu_num(fptr, &index);
  return index;
}

std::string currentName(fitsfile* fptr) {
  try {
    return Header::parseRecord<std::string>(fptr, "EXTNAME");
  } catch(const std::exception& e) {
    // EXTNAME not provided
    return "";
  }
}

Type currentType(fitsfile* fptr) {
  int type = 0;
  int status = 0;
  fits_get_hdu_type(fptr, &type, &status);
  if(type == BINARY_TBL)
    return Type::Bintable;
  return Type::Image;
}

bool currentIsPrimary(fitsfile* fptr) {
  return currentIndex(fptr) == 1;
}

bool gotoIndex(fitsfile* fptr, long index) {
  if(index == currentIndex(fptr))
    return false;
  int type = 0;
  int status = 0;
  fits_movabs_hdu(fptr, static_cast<int>(index), &type, &status); // HDU indices are int
  mayThrowCfitsioError(status, "Cannot access HDU " + std::to_string(index));
  return true;
}

bool gotoName(fitsfile* fptr, const std::string& name) {
  if(name == "")
    return false;
  if(name == "Primary")
    return gotoPrimary(fptr);
  if(name == currentName(fptr))
    return false;
  int status = 0;
  fits_movnam_hdu(fptr, ANY_HDU, toCharPtr(name).get(), 0, &status);
  mayThrowCfitsioError(status);
  return true;
}

bool gotoNext(fitsfile* fptr, long step) {
  if(step == 0)
    return false;
  int status = 0;
  int type = 0;
  fits_movrel_hdu(fptr, static_cast<int>(step), &type, &status); // HDU indices are int
  mayThrowCfitsioError(status);
  return true;
}

bool gotoPrimary(fitsfile* fptr) {
  return gotoIndex(fptr, 1);
}

bool initPrimary(fitsfile* fptr) {
  if(count(fptr) > 0)
    return false;
  createMetadataExtension(fptr, "");
  return true;
}

bool updateName(fitsfile* fptr, const std::string& name) {
  if(name == "")
    return false;
  Header::updateRecord(fptr, FitsIO::Record<std::string>("EXTNAME", name));
  return true;
}

void createMetadataExtension(fitsfile* fptr, const std::string& name) {
  createImageExtension<unsigned char, 0>(fptr, name, FitsIO::pos_type<0>());
}

void delete_hdu(fitsfile *fptr, long index) {
  gotoIndex(fptr, index);
  int status = 0;
  fits_delete_hdu(fptr, nullptr, &status);
  mayThrowCfitsioError(status, "Cannot delete HDU " + std::to_string(index));
}

}
}
}
