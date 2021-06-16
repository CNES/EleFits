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

RecordHdu::RecordHdu(Token, fitsfile*& fptr, long index, HduCategory type) :
    m_fptr(fptr), m_cfitsioIndex(index + 1), m_type(type) {}

RecordHdu::RecordHdu() : m_fptr(m_dummyFptr), m_cfitsioIndex(0), m_type(HduCategory::Image) {}

long RecordHdu::index() const {
  return m_cfitsioIndex - 1;
}

HduCategory RecordHdu::type() const {
  return m_type;
}

HduCategory RecordHdu::readCategory() const {
  HduCategory cat = m_type;
  if (m_cfitsioIndex == 1) {
    cat &= HduCategory::Primary;
  } else {
    cat &= HduCategory::Ext;
  }
  return cat;
}

bool RecordHdu::matches(HduFilter filter) const {
  return filter.accepts(readCategory());
}

std::string RecordHdu::readName() const {
  gotoThisHdu();
  return Cfitsio::Hdu::currentName(m_fptr);
}

void RecordHdu::updateName(const std::string& name) const {
  gotoThisHdu();
  Cfitsio::Hdu::updateName(m_fptr, name);
}

std::string RecordHdu::readHeader(bool incNonValued) const {
  gotoThisHdu();
  return Cfitsio::Header::readHeader(m_fptr, incNonValued);
}

std::vector<std::string> RecordHdu::readKeywords(KeywordCategory categories) const {
  return Cfitsio::Header::listKeywords(m_fptr, categories);
}

std::map<std::string, std::string> RecordHdu::readKeywordsValues(KeywordCategory categories) const {
  return Cfitsio::Header::listKeywordsValues(m_fptr, categories);
}

bool RecordHdu::hasKeyword(const std::string& keyword) const {
  return Cfitsio::Header::hasKeyword(m_fptr, keyword);
}

void RecordHdu::writeComment(const std::string& comment) const {
  return Cfitsio::Header::writeComment(m_fptr, comment);
}

void RecordHdu::writeHistory(const std::string& history) const {
  return Cfitsio::Header::writeHistory(m_fptr, history);
}

void RecordHdu::deleteRecord(const std::string& keyword) const {
  gotoThisHdu();
  Cfitsio::Header::deleteRecord(m_fptr, keyword);
}

void RecordHdu::gotoThisHdu() const {
  Cfitsio::Hdu::gotoIndex(m_fptr, m_cfitsioIndex);
}

#ifndef COMPILE_PARSE_RECORD
  #define COMPILE_PARSE_RECORD(type, unused) template Record<type> RecordHdu::parseRecord(const std::string&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_PARSE_RECORD)
  #undef COMPILE_PARSE_RECORD
#endif

template RecordVector<boost::any> RecordHdu::parseRecordVector(const std::vector<std::string>&) const;

#ifndef COMPILE_WRITE_RECORD
  #define COMPILE_WRITE_RECORD(type, unused) \
    template void RecordHdu::writeRecord(const Record<type>&) const; \
    template void RecordHdu::updateRecord(const Record<type>&) const;
EL_FITSIO_FOREACH_RECORD_TYPE(COMPILE_WRITE_RECORD)
  #undef COMPILE_WRITE_RECORD
#endif

template void RecordHdu::writeRecords(const std::vector<Record<boost::any>>&) const;
template void RecordHdu::updateRecords(const std::vector<Record<boost::any>>&) const;

} // namespace FitsIO
} // namespace Euclid
