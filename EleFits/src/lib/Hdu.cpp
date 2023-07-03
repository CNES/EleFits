// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/Hdu.h"

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"

namespace Euclid {
namespace Fits {

Hdu::Hdu(Token, fitsfile*& fptr, long index, HduCategory type, HduCategory status) :
    m_fptr(fptr), m_cfitsioIndex(index + 1), m_type(type),
    m_header(
        m_fptr,
        [&]() {
          touchThisHdu();
        },
        [&]() {
          editThisHdu();
        }),
    m_status(status) {}

Hdu::Hdu() : Hdu(Token(), m_dummyFptr, 0, HduCategory::Image, HduCategory::Untouched) {}

long Hdu::index() const {
  return m_cfitsioIndex - 1;
}

HduCategory Hdu::type() const {
  return m_type;
}

HduCategory Hdu::readCategory() const {
  touchThisHdu();
  HduCategory cat = m_type & m_status;
  if (m_cfitsioIndex == 1) {
    cat &= HduCategory::Primary;
  } else {
    cat &= HduCategory::Ext;
  }
  return cat;
}

const Header& Hdu::header() const {
  return m_header;
}

bool Hdu::matches(HduFilter filter) const {
  return filter.accepts(readCategory());
}

std::string Hdu::readName() const {
  touchThisHdu();
  return Cfitsio::HduAccess::currentName(m_fptr);
}

long Hdu::readVersion() const {
  touchThisHdu();
  return Cfitsio::HduAccess::currentVersion(m_fptr);
}

long Hdu::readDataUnitSize() const {
  touchThisHdu();
  long bitpix = m_header.parse<long>("BITPIX");
  long gcount = m_header.parseOr<long>("GCOUNT", 1);
  long pcount = m_header.parseOr<long>("PCOUNT", 0);
  long naxis = m_header.parse<long>("NAXIS");
  long pixelCount;
  if (naxis > 0) {
    pixelCount = 1;
    for (int i = 1; i <= naxis; i++) {
      pixelCount *= m_header.parse<long>("NAXIS" + std::to_string(i));
    }
  } else {
    pixelCount = 0;
  }
  return (std::abs(bitpix) * gcount * (pcount + pixelCount)) / 8;
}

void Hdu::updateName(const std::string& name) const {
  editThisHdu();
  Cfitsio::HduAccess::updateName(m_fptr, name);
}

void Hdu::updateVersion(long version) const {
  editThisHdu();
  Cfitsio::HduAccess::updateVersion(m_fptr, version);
}

void Hdu::verifyChecksums() const {
  touchThisHdu();
  int status = 0;
  int datastatus;
  int hdustatus;
  fits_verify_chksum(m_fptr, &datastatus, &hdustatus, &status);
  ChecksumError::mayThrow(ChecksumError::Status(hdustatus), ChecksumError::Status(datastatus));
  // TODO wrap in EleCfitsioWrapper
}

void Hdu::updateChecksums() const {
  editThisHdu();
  int status = 0;
  fits_write_chksum(m_fptr, &status);
  Cfitsio::CfitsioError::mayThrow(status, m_fptr, "Cannot write checksums.");
  // TODO wrap in EleCfitsioWrapper
}

void Hdu::touchThisHdu() const {
  Cfitsio::HduAccess::gotoIndex(m_fptr, m_cfitsioIndex);
  if (m_status == HduCategory::Untouched) {
    m_status = HduCategory::Touched;
  }
}

void Hdu::editThisHdu() const {
  touchThisHdu();
  m_status &= HduCategory::Edited;
}

template <>
const Header& Hdu::as() const {
  return as<Hdu>().header();
}

} // namespace Fits
} // namespace Euclid
