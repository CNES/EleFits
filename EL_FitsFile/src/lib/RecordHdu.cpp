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

RecordHdu::RecordHdu(fitsfile*& fptr, long index) :
    m_fptr(fptr), m_index(index) {}

long RecordHdu::index() const {
  return m_index;
}

std::string RecordHdu::name() const {
  goto_this_hdu();
  return Cfitsio::Hdu::current_name(m_fptr);
}

void RecordHdu::rename(std::string name) const {
  goto_this_hdu();
  Cfitsio::Hdu::update_name(m_fptr, name);
}

std::vector<std::string> RecordHdu::keywords() const {
  return Cfitsio::Header::list_keywords(m_fptr);
}

void RecordHdu::delete_record(std::string keyword) const {
  goto_this_hdu();
  Cfitsio::Header::delete_record(m_fptr, keyword);
}

void RecordHdu::goto_this_hdu() const {
  Cfitsio::Hdu::goto_index(m_fptr, m_index);
}

#ifndef COMPILE_PARSE_RECORD
#define COMPILE_PARSE_RECORD(T) \
  template Record<T> RecordHdu::parse_record(std::string) const;
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
#define COMPILE_WRITE_RECORD(T) \
  template void RecordHdu::write_record(const Record<T>&) const;
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

}
}
