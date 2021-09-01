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

#include "EL_FitsFile/BintableColumns.h"

namespace Euclid {
namespace FitsIO {

BintableColumns::BintableColumns(
    fitsfile*& fptr,
    std::function<void(void)> touchFunc,
    std::function<void(void)> editFunc) :
    m_fptr(fptr),
    m_touch(touchFunc), m_edit(editFunc) {}

long BintableColumns::readColumnCount() const {
  m_touch();
  return Cfitsio::BintableIo::columnCount(m_fptr);
}

long BintableColumns::readRowCount() const {
  m_touch();
  return Cfitsio::BintableIo::rowCount(m_fptr);
}

bool BintableColumns::has(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::hasColumn(m_fptr, name);
}

long BintableColumns::readIndex(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::columnIndex(m_fptr, name) - 1;
}

std::string BintableColumns::readName(long index) const {
  m_touch();
  return Cfitsio::BintableIo::columnName(m_fptr, index + 1);
}

std::vector<std::string> BintableColumns::readAllNames() const {
  const auto size = readColumnCount();
  std::vector<std::string> names(size);
  for (long i = 0; i < size; ++i) {
    names[i] = readName(i);
  }
  return names;
}

void BintableColumns::rename(const std::string& name, const std::string& newName) const {
  rename(readIndex(name), newName);
}

void BintableColumns::rename(long index, const std::string& newName) const {
  m_edit();
  Cfitsio::BintableIo::updateColumnName(m_fptr, index + 1, newName);
}

void BintableColumns::remove(const std::string& name) const {
  remove(readIndex(name));
}

void BintableColumns::remove(long index) const {
  m_edit();
  // FIXME implement
}

} // namespace FitsIO
} // namespace Euclid