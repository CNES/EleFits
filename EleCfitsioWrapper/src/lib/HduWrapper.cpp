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

Linx::Index count(fitsfile* fptr)
{
  int count = 0;
  int status = 0;
  fits_get_num_hdus(fptr, &count, &status);
  CfitsioError::may_throw(status, fptr, "Cannot count HDUs");
  return count;
}

Linx::Index current_index(fitsfile* fptr)
{
  int index = 0;
  fits_get_hdu_num(fptr, &index);
  return index;
}

std::string current_name(fitsfile* fptr)
{
  if (HeaderIo::has_keyword(fptr, "EXTNAME")) {
    return HeaderIo::parse_record<std::string>(fptr, "EXTNAME");
  }
  if (HeaderIo::has_keyword(fptr, "HDUNAME")) {
    return HeaderIo::parse_record<std::string>(fptr, "HDUNAME");
  }
  return "";
}

long current_version(fitsfile* fptr)
{
  if (HeaderIo::has_keyword(fptr, "EXTVER")) {
    return HeaderIo::parse_record<long>(fptr, "EXTVER");
  }
  if (HeaderIo::has_keyword(fptr, "HDUVER")) {
    return HeaderIo::parse_record<long>(fptr, "HDUVER");
  }
  return 1;
}

std::size_t current_size(fitsfile* fptr)
{
  int status = 0;
  LONGLONG current_head_start;
  LONGLONG next_head_start;

  const auto index = current_index(fptr);

  if (index < count(fptr)) { // its not the the last hdu
    fits_get_hduaddrll(fptr, &current_head_start, nullptr, nullptr, &status);
    CfitsioError::may_throw(status, fptr, "Cannot get Hdu address");
    fits_movrel_hdu(fptr, static_cast<int>(1), nullptr, &status); // HDU indices are int
    CfitsioError::may_throw(status, fptr, "Cannot move to next HDU (step +1)");
    fits_get_hduaddrll(fptr, &next_head_start, nullptr, nullptr, &status);
    CfitsioError::may_throw(status, fptr, "Cannot get Hdu address");

  } else { // its the last hdu
    // dataend arg (next_head_start) should be the end of file:
    fits_get_hduaddrll(fptr, &current_head_start, nullptr, &next_head_start, &status);
    CfitsioError::may_throw(status, fptr, "Cannot get Hdu address");
  }

  // return to current hdu
  goto_index(fptr, index);

  return static_cast<std::size_t>(next_head_start - current_head_start);
}

Fits::HduCategory current_type(fitsfile* fptr)
{
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

bool current_is_primary(fitsfile* fptr)
{
  return current_index(fptr) == 1;
}

bool goto_index(fitsfile* fptr, Linx::Index index)
{
  if (index == current_index(fptr)) {
    return false;
  }
  int type = 0;
  int status = 0;
  fits_movabs_hdu(fptr, static_cast<int>(index), &type, &status); // HDU indices are int
  CfitsioError::may_throw(status, fptr, "Cannot access HDU: #" + std::to_string(index - 1));
  return true;
}

bool goto_name(fitsfile* fptr, const std::string& name, long version, Fits::HduCategory category)
{
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
  fits_movnam_hdu(fptr, hdutype, Fits::String::to_char_ptr(name).get(), version, &status);
  CfitsioError::may_throw(status, fptr, "Cannot move to HDU: " + name);
  return true;
}

bool goto_next(fitsfile* fptr, Linx::Index step)
{
  if (step == 0) {
    return false;
  }
  int status = 0;
  int type = 0;
  fits_movrel_hdu(fptr, static_cast<int>(step), &type, &status); // HDU indices are int
  CfitsioError::may_throw(status, fptr, "Cannot move to next HDU (step " + std::to_string(step) + ")");
  return true;
}

bool goto_primary(fitsfile* fptr)
{
  return goto_index(fptr, 1);
}

bool init_primary(fitsfile* fptr)
{
  if (count(fptr) > 0) {
    return false;
  }
  init_image<unsigned char, 0>(fptr, "", Linx::Position<0>());
  return true;
}

bool update_name(fitsfile* fptr, const std::string& name)
{
  if (name == "") {
    return false;
  }
  HeaderIo::update_record(fptr, Fits::Record<std::string>("EXTNAME", name));
  return true;
}

bool update_version(fitsfile* fptr, long version)
{
  if (version == 0) {
    return false;
  }
  HeaderIo::update_record(fptr, Fits::Record<long>("EXTVER", version));
  return true;
}

void copy_verbatim(fitsfile* from, fitsfile* to)
{
  int status = 0;
  fits_copy_hdu(from, to, 0, &status);
  Cfitsio::CfitsioError::may_throw(status, to, "Cannot copy HDU");
}

void remove(fitsfile* fptr, Linx::Index index)
{
  goto_index(fptr, index);
  int status = 0;
  fits_delete_hdu(fptr, nullptr, &status);
  CfitsioError::may_throw(status, fptr, "Cannot delete HDU: " + std::to_string(index - 1));
}

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid
