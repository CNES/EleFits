/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "EleFits/BintableColumns.h"

namespace Euclid {
namespace Fits {

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

long BintableColumns::readBufferRowCount() const {
  long size = 0;
  int status = 0;
  fits_get_rowsize(m_fptr, &size, &status);
  Cfitsio::CfitsioError::mayThrow(status, m_fptr, "Cannot compute buffer row count.");
  return size;
}

bool BintableColumns::has(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::hasColumn(m_fptr, name);
}

long BintableColumns::readIndex(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::columnIndex(m_fptr, name) - 1;
}

std::vector<long> BintableColumns::readIndices(const std::vector<std::string>& names) const {
  m_touch();
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return Cfitsio::BintableIo::columnIndex(m_fptr, n) - 1;
  });
  return indices;
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

void BintableColumns::rename(ColumnKey key, const std::string& newName) const {
  m_edit();
  Cfitsio::BintableIo::updateColumnName(m_fptr, key.index(*this) + 1, newName);
}

void BintableColumns::remove(ColumnKey key) const {
  m_edit();
  int status = 0;
  fits_delete_col(m_fptr, key.index(*this) + 1, &status);
  Cfitsio::CfitsioError::mayThrow(status, m_fptr, "Cannot remove column #" + std::to_string(key.index(*this)));
  // TODO to Cfitsio
}

void BintableColumns::removeSeq(std::vector<ColumnKey> keys) const {
  // FIXME Order indices in descending order
  for (auto& k : keys) {
    remove(std::move(k)); // TODO Scans the whole HDU at every loop
  }
}

} // namespace Fits
} // namespace Euclid
