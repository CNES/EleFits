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

#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {
namespace FitsIO {

BintableHdu::BintableHdu(Token token, fitsfile*& fptr, long index) :
    RecordHdu(token, fptr, index, HduCategory::Bintable) {}

long BintableHdu::readColumnCount() const {
  gotoThisHdu();
  return Cfitsio::Bintable::columnCount(m_fptr);
}

long BintableHdu::readRowCount() const {
  gotoThisHdu();
  return Cfitsio::Bintable::rowCount(m_fptr);
}

bool BintableHdu::hasColumn(const std::string& name) const {
  gotoThisHdu();
  return Cfitsio::Bintable::hasColumn(m_fptr, name);
}

std::vector<bool> BintableHdu::hasColumns(const std::vector<std::string>& names) const {
  gotoThisHdu();
  const auto size = names.size();
  std::vector<bool> counts(size);
  std::transform(names.begin(), names.end(), counts.begin(), [&](const std::string& n) {
    return Cfitsio::Bintable::hasColumn(m_fptr, n);
  });
  return counts;
}

long BintableHdu::readColumnIndex(const std::string& name) const {
  gotoThisHdu();
  return Cfitsio::Bintable::columnIndex(m_fptr, name) - 1;
}

std::string BintableHdu::readColumnName(long index) const {
  gotoThisHdu();
  return Cfitsio::Bintable::columnName(m_fptr, index + 1);
}

std::vector<std::string> BintableHdu::readColumnNames() const {
  const auto size = readColumnCount(); // calls gotoThisHdu
  std::vector<std::string> names(size);
  for (long i = 0; i < size; ++i) {
    names[i] = Cfitsio::Bintable::columnName(m_fptr, i + 1);
  }
  return names;
}

void BintableHdu::renameColumn(const std::string& name, const std::string& newName) const {
  renameColumn(readColumnIndex(name), newName);
}

void BintableHdu::renameColumn(long index, const std::string& newName) const {
  gotoThisHdu();
  Cfitsio::Bintable::updateColumnName(m_fptr, index + 1, newName);
}

#ifndef COMPILE_READ_COLUMN
  #define COMPILE_READ_COLUMN(type, unused) template VecColumn<type> BintableHdu::readColumn(const std::string&) const;
EL_FITSIO_FOREACH_COLUMN_TYPE(COMPILE_READ_COLUMN)
  #undef COMPILE_READ_COLUMN
#endif

#ifndef COMPILE_WRITE_COLUMN
  #define COMPILE_WRITE_COLUMN(type, unused) template void BintableHdu::writeColumn(const Column<type>&) const;
EL_FITSIO_FOREACH_COLUMN_TYPE(COMPILE_WRITE_COLUMN)
  #undef COMPILE_WRITE_COLUMN
#endif

} // namespace FitsIO
} // namespace Euclid
