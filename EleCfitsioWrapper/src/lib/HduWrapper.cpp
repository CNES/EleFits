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

#include "EleCfitsioWrapper/HduWrapper.h"

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace HduAccess {

long count(fitsfile* fptr) {
  int count = 0;
  int status = 0;
  fits_get_num_hdus(fptr, &count, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot count HDUs");
  return count;
}

long currentIndex(fitsfile* fptr) {
  int index = 0;
  fits_get_hdu_num(fptr, &index);
  return index;
}

std::string currentName(fitsfile* fptr) {
  if (HeaderIo::hasKeyword(fptr, "EXTNAME")) {
    return HeaderIo::parseRecord<std::string>(fptr, "EXTNAME");
  }
  if (HeaderIo::hasKeyword(fptr, "HDUNAME")) {
    return HeaderIo::parseRecord<std::string>(fptr, "HDUNAME");
  }
  return "";
}

long currentVersion(fitsfile* fptr) {
  if (HeaderIo::hasKeyword(fptr, "EXTVER")) {
    return HeaderIo::parseRecord<long>(fptr, "EXTVER");
  }
  return 1;
}

Fits::HduCategory currentType(fitsfile* fptr) {
  int type = 0;
  int status = 0;
  fits_get_hdu_type(fptr, &type, &status);
  if (type == IMAGE_HDU) {
    return Fits::HduCategory::Image;
  }
  if (type == BINARY_TBL) {
    return Fits::HduCategory::Bintable;
  }
  throw Fits::FitsError("Unknown HDU type (code " + std::to_string(type) + ").");
}

bool currentIsPrimary(fitsfile* fptr) {
  return currentIndex(fptr) == 1;
}

bool gotoIndex(fitsfile* fptr, long index) {
  if (index == currentIndex(fptr)) {
    return false;
  }
  int type = 0;
  int status = 0;
  fits_movabs_hdu(fptr, static_cast<int>(index), &type, &status); // HDU indices are int
  CfitsioError::mayThrow(status, fptr, "Cannot access HDU: #" + std::to_string(index - 1));
  return true;
}

bool gotoName(fitsfile* fptr, const std::string& name, long version, Fits::HduCategory category) {
  if (name == "") {
    return false;
  }
  int status = 0;
  int hdutype = ANY_HDU;
  if (category == Fits::HduCategory::Image) {
    hdutype = IMAGE_HDU;
  } else if (category == Fits::HduCategory::Bintable) {
    hdutype = BINARY_TBL;
  } else if (category != Fits::HduCategory::Any) {
    throw Fits::FitsError("Invalid HduCategory; Only Any, Image and Bintable are supported.");
  }
  fits_movnam_hdu(fptr, hdutype, toCharPtr(name).get(), version, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot move to HDU: " + name);
  return true;
}

bool gotoNext(fitsfile* fptr, long step) {
  if (step == 0) {
    return false;
  }
  int status = 0;
  int type = 0;
  fits_movrel_hdu(fptr, static_cast<int>(step), &type, &status); // HDU indices are int
  CfitsioError::mayThrow(status, fptr, "Cannot move to next HDU (step " + std::to_string(step) + ")");
  return true;
}

bool gotoPrimary(fitsfile* fptr) {
  return gotoIndex(fptr, 1);
}

bool initPrimary(fitsfile* fptr) {
  if (count(fptr) > 0) {
    return false;
  }
  createMetadataExtension(fptr, "");
  return true;
}

bool updateName(fitsfile* fptr, const std::string& name) {
  if (name == "") {
    return false;
  }
  HeaderIo::updateRecord(fptr, Fits::Record<std::string>("EXTNAME", name));
  return true;
}

bool updateVersion(fitsfile* fptr, long version) {
  if (version == 0) {
    return false;
  }
  HeaderIo::updateRecord(fptr, Fits::Record<long>("EXTVER", version));
  return true;
}

void createMetadataExtension(fitsfile* fptr, const std::string& name) {
  createImageExtension<unsigned char, 0>(fptr, name, Fits::Position<0>());
}

void deleteHdu(fitsfile* fptr, long index) {
  gotoIndex(fptr, index);
  int status = 0;
  fits_delete_hdu(fptr, nullptr, &status);
  CfitsioError::mayThrow(status, fptr, "Cannot delete HDU: " + std::to_string(index - 1));
}

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid
