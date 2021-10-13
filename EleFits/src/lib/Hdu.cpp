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
namespace FitsIO {

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

std::string Hdu::readHeader(bool incNonValued) const {
  touchThisHdu();
  return Cfitsio::HeaderIo::readHeader(m_fptr, incNonValued);
}

std::vector<std::string> Hdu::readKeywords(KeywordCategory categories) const {
  return m_header.readKeywords(categories);
}

std::map<std::string, std::string> Hdu::readKeywordsValues(KeywordCategory categories) const {
  return m_header.readKeywordsValues(categories);
}

bool Hdu::hasKeyword(const std::string& keyword) const {
  touchThisHdu();
  return Cfitsio::HeaderIo::hasKeyword(m_fptr, keyword);
}

RecordSeq Hdu::parseRecordSeq(const std::vector<std::string>& keywords) const {
  return parseRecordVector<VariantValue>(keywords);
}

void Hdu::writeComment(const std::string& comment) const {
  editThisHdu();
  return Cfitsio::HeaderIo::writeComment(m_fptr, comment);
}

void Hdu::writeHistory(const std::string& history) const {
  editThisHdu();
  return Cfitsio::HeaderIo::writeHistory(m_fptr, history);
}

void Hdu::deleteRecord(const std::string& keyword) const {
  editThisHdu();
  KeywordNotFoundError::mayThrow(keyword, m_header);
  Cfitsio::HeaderIo::deleteRecord(m_fptr, keyword);
}

void Hdu::verifyChecksums() const {
  touchThisHdu();
  int status = 0;
  int datastatus;
  int hdustatus;
  fits_verify_chksum(m_fptr, &datastatus, &hdustatus, &status);
  // FIXME wrap in EleCfitsioWrapper and throw if needs be
  ChecksumError::mayThrow(ChecksumError::Status(hdustatus), ChecksumError::Status(datastatus));
}

void Hdu::updateChecksums() const {
  editThisHdu();
  int status = 0;
  fits_write_chksum(m_fptr, &status);
  // FIXME wrap in EleCfitsioWrapper and throw if needs be
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

#ifndef COMPILE_PARSE_RECORD
  #define COMPILE_PARSE_RECORD(type, unused) template Record<type> Hdu::parseRecord(const std::string&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_PARSE_RECORD)
  #undef COMPILE_PARSE_RECORD
#endif

template RecordSeq Hdu::parseRecordVector(const std::vector<std::string>&) const;

#ifndef COMPILE_WRITE_RECORD
  #define COMPILE_WRITE_RECORD(type, unused) template void Hdu::writeRecord(const Record<type>&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_WRITE_RECORD)
  #undef COMPILE_WRITE_RECORD
#endif

template void Hdu::writeRecords(const std::vector<Record<VariantValue>>&) const;

} // namespace FitsIO
} // namespace Euclid
