// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/HduWrapper.h"

#include "EleCfitsioWrapper/ErrorWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/Raster.h"

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
  if (HeaderIo::hasKeyword(fptr, "HDUVER")) {
    return HeaderIo::parseRecord<long>(fptr, "HDUVER");
  }
  return 1;
}

std::size_t currentSize(fitsfile* fptr) {
  int status = 0;
  LONGLONG currentHeadStart;
  LONGLONG nextHeadStart;

  long currentIdx = currentIndex(fptr);

  if (currentIdx < count(fptr)) { // its not the the last hdu
    fits_get_hduaddrll(fptr, &currentHeadStart, nullptr, nullptr, &status);
    CfitsioError::mayThrow(status, fptr, "Cannot get Hdu address");
    fits_movrel_hdu(fptr, static_cast<int>(1), nullptr, &status); // HDU indices are int
    CfitsioError::mayThrow(status, fptr, "Cannot move to next HDU (step +1)");
    fits_get_hduaddrll(fptr, &nextHeadStart, nullptr, nullptr, &status);
    CfitsioError::mayThrow(status, fptr, "Cannot get Hdu address");

  } else { // its the last hdu
    // dataend arg (nextHeadStart) should be the end of file:
    fits_get_hduaddrll(fptr, &currentHeadStart, nullptr, &nextHeadStart, &status);
    CfitsioError::mayThrow(status, fptr, "Cannot get Hdu address");
  }

  // return to current hdu
  gotoIndex(fptr, currentIdx);

  return static_cast<std::size_t>(nextHeadStart - currentHeadStart);
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
  fits_movnam_hdu(fptr, hdutype, Fits::String::toCharPtr(name).get(), version, &status);
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
  initImageExtension<unsigned char, 0>(fptr, name, Fits::Position<0>());
}

void binaryCopy(fitsfile* srcFptr, fitsfile* dstFptr) {
  int status = 0;
  fits_copy_hdu(srcFptr, dstFptr, 0, &status);
  Cfitsio::CfitsioError::mayThrow(status, dstFptr, "Cannot copy hdu");
}

#define ARE_SAME_TYPEID(type, name) \
  if (typeid(type) == Cfitsio::ImageIo::readTypeid(fptr)) { \
    return Cfitsio::TypeCode<type>::forImage(); \
  }

int readTypeCode(fitsfile* fptr) {
  ELEFITS_FOREACH_RASTER_TYPE(ARE_SAME_TYPEID)
  throw Fits::FitsError("Unrecognized datatype for Hdu copy.");
}

// FIXME: use CFitsioWrapper functions & abstractions for improved robustness
void contextualCopy(fitsfile* srcFptr, fitsfile* dstFptr) {

  int status = 0, ii = 0, naxis = 0, datatype = 0, bitpix, nkeys, anynul;
  double nulval = 0.0;
  long naxes[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  char card[81];

  // Get image dimensions and total number of pixels in image
  fits_get_img_param(srcFptr, 9, &bitpix, &naxis, naxes, &status);
  Cfitsio::CfitsioError::mayThrow(status, srcFptr, "Cannot get img params");
  long totpix = naxes[0] * naxes[1] * naxes[2] * naxes[3] * naxes[4] * naxes[5] * naxes[6] * naxes[7] * naxes[8];

  // Explicitly create new image, to support compression
  fits_create_img(dstFptr, bitpix, naxis, naxes, &status);
  Cfitsio::CfitsioError::mayThrow(status, dstFptr, "Cannot create img");

  // Copy all the user keywords (not the structural keywords)
  fits_get_hdrspace(srcFptr, &nkeys, NULL, &status);
  Cfitsio::CfitsioError::mayThrow(status, srcFptr, "Cannot get hdrspace");

  for (ii = 1; ii <= nkeys; ii++) {
    fits_read_record(srcFptr, ii, card, &status);
    Cfitsio::CfitsioError::mayThrow(status, srcFptr, "Cannot read record");
    if (fits_get_keyclass(card) > TYP_CMPRS_KEY)
      fits_write_record(dstFptr, card, &status);
    Cfitsio::CfitsioError::mayThrow(status, srcFptr, "Cannot write record");
  }

  datatype = readTypeCode(srcFptr);

  const int bytepix = std::abs(bitpix) / 8;

  const long npix = totpix;

  // No scaling desabling required here, because all datatype (specifically unsigned types as well) are taken into account

  // Allocate memory for the entire image (use double type to force memory alignment)
  std::vector<double> array(npix * bytepix / sizeof(double));

  int comptype; // used for specific PLIO compression case
  fits_get_compression_type(dstFptr, &comptype, &status);
  Cfitsio::CfitsioError::mayThrow(status, dstFptr, "Cannot get compression type");

  const long first = 1;

  // Read all or part of image then write it back to the output file
  fits_read_img(srcFptr, datatype, first, npix, &nulval, array.data(), &anynul, &status);
  Cfitsio::CfitsioError::mayThrow(status, srcFptr, "Cannot read img");

  fits_write_img(dstFptr, datatype, first, npix, array.data(), &status);
  Cfitsio::CfitsioError::mayThrow(status, dstFptr, "Cannot write img");
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
