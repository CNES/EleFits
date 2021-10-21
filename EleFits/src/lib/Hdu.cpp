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

#include "EleFits/Hdu.h"

#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace Fits {

Hdu::Hdu(Token, fitsfile*& fptr, long index, HduCategory type, HduCategory status) :
    m_fptr(fptr), m_cfitsioIndex(index + 1), m_type(type), m_header(
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

} // namespace Fits
} // namespace Euclid
