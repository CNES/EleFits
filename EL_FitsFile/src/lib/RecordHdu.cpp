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

RecordHdu::RecordHdu(fitsfile *&fptr, long index) : m_fptr(fptr), m_index(index) {
}

long RecordHdu::index() const {
  return m_index;
}

std::string RecordHdu::name() const {
  gotoThisHdu();
  return Cfitsio::Hdu::currentName(m_fptr);
}

void RecordHdu::rename(const std::string &name) const {
  gotoThisHdu();
  Cfitsio::Hdu::updateName(m_fptr, name);
}

std::vector<std::string> RecordHdu::keywords() const {
  return Cfitsio::Header::listKeywords(m_fptr);
}

void RecordHdu::deleteRecord(const std::string &keyword) const {
  gotoThisHdu();
  Cfitsio::Header::deleteRecord(m_fptr, keyword);
}

void RecordHdu::gotoThisHdu() const {
  Cfitsio::Hdu::gotoIndex(m_fptr, m_index);
}

#ifndef COMPILE_PARSE_RECORD
#define COMPILE_PARSE_RECORD(T) template Record<T> RecordHdu::parseRecord(const std::string &) const;
COMPILE_PARSE_RECORD(char)
COMPILE_PARSE_RECORD(short)
COMPILE_PARSE_RECORD(int)
COMPILE_PARSE_RECORD(long)
COMPILE_PARSE_RECORD(float)
COMPILE_PARSE_RECORD(double)
COMPILE_PARSE_RECORD(unsigned char)
COMPILE_PARSE_RECORD(unsigned short)
COMPILE_PARSE_RECORD(unsigned int)
COMPILE_PARSE_RECORD(unsigned long)
#undef COMPILE_PARSE_RECORD
#endif

#ifndef COMPILE_WRITE_RECORD
#define COMPILE_WRITE_RECORD(T) template void RecordHdu::writeRecord(const Record<T> &) const;
COMPILE_WRITE_RECORD(char)
COMPILE_WRITE_RECORD(short)
COMPILE_WRITE_RECORD(int)
COMPILE_WRITE_RECORD(long)
COMPILE_WRITE_RECORD(float)
COMPILE_WRITE_RECORD(double)
COMPILE_WRITE_RECORD(unsigned char)
COMPILE_WRITE_RECORD(unsigned short)
COMPILE_WRITE_RECORD(unsigned int)
COMPILE_WRITE_RECORD(unsigned long)
#undef COMPILE_WRITE_RECORD
#endif

} // namespace FitsIO
} // namespace Euclid
