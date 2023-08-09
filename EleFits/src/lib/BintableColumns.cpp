// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableColumns.h"

#include <algorithm> // sort

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
  return Cfitsio::BintableIo::column_count(m_fptr);
}

long BintableColumns::readRowCount() const {
  m_touch();
  return Cfitsio::BintableIo::row_count(m_fptr);
}

long BintableColumns::readBufferRowCount() const {
  long size = 0;
  int status = 0;
  fits_get_rowsize(m_fptr, &size, &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot compute buffer row count.");
  return size;
}

bool BintableColumns::has(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::has_column(m_fptr, name);
}

long BintableColumns::readIndex(const std::string& name) const {
  m_touch();
  return Cfitsio::BintableIo::column_index(m_fptr, name) - 1;
}

std::vector<long> BintableColumns::readIndices(const std::vector<std::string>& names) const {
  m_touch();
  std::vector<long> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return Cfitsio::BintableIo::column_index(m_fptr, n) - 1;
  });
  return indices;
}

std::string BintableColumns::readName(long index) const {
  m_touch();
  return Cfitsio::BintableIo::column_name(m_fptr, index + 1);
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
  Cfitsio::BintableIo::update_column_name(m_fptr, key.index(*this) + 1, newName);
}

void BintableColumns::remove(ColumnKey key) const {
  m_edit();
  int status = 0;
  fits_delete_col(m_fptr, key.index(*this) + 1, &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot remove column #" + std::to_string(key.index(*this)));
  // TODO to Cfitsio
}

void BintableColumns::removeSeq(std::vector<ColumnKey> keys) const {
  std::sort(keys.begin(), keys.end(), [&](auto& lhs, auto& rhs) {
    return lhs.index(*this) > rhs.index(*this); // descending order to avoid shifting
  });
  for (auto& k : keys) {
    remove(std::move(k)); // WARN Scans the whole HDU at every loop
  }
}

} // namespace Fits
} // namespace Euclid
