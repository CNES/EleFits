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

#include "EL_FitsFile/RecordHdu.h"

#include "EL_CfitsioWrapper/HduWrapper.h"
#include "EL_CfitsioWrapper/HeaderWrapper.h"

namespace Euclid {
namespace FitsIO {

RecordHdu::RecordHdu(Token, fitsfile*& fptr, long index, HduCategory type, HduCategory status) :
    m_fptr(fptr), m_cfitsioIndex(index + 1), m_type(type), m_header(
                                                               m_fptr,
                                                               [&]() {
                                                                 touchThisHdu();
                                                               },
                                                               [&]() {
                                                                 editThisHdu();
                                                               }),
    m_status(status) {}

RecordHdu::RecordHdu() : RecordHdu(Token(), m_dummyFptr, 0, HduCategory::Image, HduCategory::Untouched) {}

long RecordHdu::index() const {
  return m_cfitsioIndex - 1;
}

HduCategory RecordHdu::type() const {
  return m_type;
}

HduCategory RecordHdu::readCategory() const {
  HduCategory cat = m_type & m_status;
  if (m_cfitsioIndex == 1) {
    cat &= HduCategory::Primary;
  } else {
    cat &= HduCategory::Ext;
  }
  return cat;
}

const Header& RecordHdu::header() const {
  return m_header;
}

bool RecordHdu::matches(HduFilter filter) const {
  return filter.accepts(readCategory());
}

std::string RecordHdu::readName() const {
  touchThisHdu();
  return Cfitsio::Hdu::currentName(m_fptr);
}

long RecordHdu::readVersion() const {
  touchThisHdu();
  return Cfitsio::Hdu::currentVersion(m_fptr);
}

void RecordHdu::updateName(const std::string& name) const {
  editThisHdu();
  Cfitsio::Hdu::updateName(m_fptr, name);
}

void RecordHdu::updateVersion(long version) const {
  editThisHdu();
  Cfitsio::Hdu::updateVersion(m_fptr, version);
}

std::string RecordHdu::readHeader(bool incNonValued) const {
  touchThisHdu();
  return Cfitsio::Header::readHeader(m_fptr, incNonValued);
}

std::vector<std::string> RecordHdu::readKeywords(KeywordCategory categories) const {
  return m_header.readKeywords(categories);
}

std::map<std::string, std::string> RecordHdu::readKeywordsValues(KeywordCategory categories) const {
  return m_header.readKeywordsValues(categories);
}

bool RecordHdu::hasKeyword(const std::string& keyword) const {
  touchThisHdu();
  return Cfitsio::Header::hasKeyword(m_fptr, keyword);
}

RecordVector<VariantValue> RecordHdu::parseRecordCollection(const std::vector<std::string>& keywords) const {
  return parseRecordVector<VariantValue>(keywords);
}

void RecordHdu::writeComment(const std::string& comment) const {
  editThisHdu();
  return Cfitsio::Header::writeComment(m_fptr, comment);
}

void RecordHdu::writeHistory(const std::string& history) const {
  editThisHdu();
  return Cfitsio::Header::writeHistory(m_fptr, history);
}

void RecordHdu::deleteRecord(const std::string& keyword) const {
  editThisHdu();
  KeywordNotFoundError::mayThrow(keyword, m_header);
  Cfitsio::Header::deleteRecord(m_fptr, keyword);
}

void RecordHdu::touchThisHdu() const {
  Cfitsio::Hdu::gotoIndex(m_fptr, m_cfitsioIndex);
  if (m_status == HduCategory::Untouched) {
    m_status = HduCategory::Touched;
  }
}

void RecordHdu::editThisHdu() const {
  touchThisHdu();
  m_status &= HduCategory::Edited;
}

#ifndef COMPILE_PARSE_RECORD
  #define COMPILE_PARSE_RECORD(type, unused) template Record<type> RecordHdu::parseRecord(const std::string&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_PARSE_RECORD)
  #undef COMPILE_PARSE_RECORD
#endif

template RecordVector<VariantValue> RecordHdu::parseRecordVector(const std::vector<std::string>&) const;

#ifndef COMPILE_WRITE_RECORD
  #define COMPILE_WRITE_RECORD(type, unused) template void RecordHdu::writeRecord(const Record<type>&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_WRITE_RECORD)
  #undef COMPILE_WRITE_RECORD
#endif

template void RecordHdu::writeRecords(const std::vector<Record<VariantValue>>&) const;

} // namespace FitsIO
} // namespace Euclid
