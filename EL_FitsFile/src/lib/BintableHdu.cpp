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

BintableHdu::BintableHdu(Token token, fitsfile*& fptr, long index, HduCategory status) :
    Hdu(token, fptr, index, HduCategory::Bintable, status), m_columns(
                                                                m_fptr,
                                                                [&]() {
                                                                  touchThisHdu();
                                                                },
                                                                [&]() {
                                                                  editThisHdu();
                                                                }) {}

BintableHdu::BintableHdu() :
    Hdu(), m_columns(
               m_fptr,
               [&]() {
                 touchThisHdu();
               },
               [&]() {
                 editThisHdu();
               }) {}

long BintableHdu::readColumnCount() const {
  touchThisHdu();
  return Cfitsio::BintableIo::columnCount(m_fptr);
}

long BintableHdu::readRowCount() const {
  touchThisHdu();
  return Cfitsio::BintableIo::rowCount(m_fptr);
}

HduCategory BintableHdu::readCategory() const {
  auto cat = Hdu::readCategory();
  if (readColumnCount() == 0 || readRowCount() == 0) {
    cat &= HduCategory::Metadata;
  } else {
    cat &= HduCategory::Data;
  }
  return cat;
}

bool BintableHdu::hasColumn(const std::string& name) const {
  touchThisHdu();
  return Cfitsio::BintableIo::hasColumn(m_fptr, name);
}

std::vector<bool> BintableHdu::hasColumns(const std::vector<std::string>& names) const {
  touchThisHdu();
  const auto size = names.size();
  std::vector<bool> counts(size);
  std::transform(names.begin(), names.end(), counts.begin(), [&](const std::string& n) {
    return Cfitsio::BintableIo::hasColumn(m_fptr, n);
  });
  return counts;
}

long BintableHdu::readColumnIndex(const std::string& name) const {
  touchThisHdu();
  return Cfitsio::BintableIo::columnIndex(m_fptr, name) - 1;
}

std::string BintableHdu::readColumnName(long index) const {
  touchThisHdu();
  return Cfitsio::BintableIo::columnName(m_fptr, index + 1);
}

std::vector<std::string> BintableHdu::readColumnNames() const {
  const auto size = readColumnCount(); // calls touchThisHdu
  std::vector<std::string> names(size);
  for (long i = 0; i < size; ++i) {
    names[i] = Cfitsio::BintableIo::columnName(m_fptr, i + 1);
  }
  return names;
}

void BintableHdu::renameColumn(const std::string& name, const std::string& newName) const {
  renameColumn(readColumnIndex(name), newName);
}

void BintableHdu::renameColumn(long index, const std::string& newName) const {
  editThisHdu();
  Cfitsio::BintableIo::updateColumnName(m_fptr, index + 1, newName);
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
