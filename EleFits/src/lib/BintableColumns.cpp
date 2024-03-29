// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableColumns.h"

#include <algorithm> // sort

namespace Fits {

BintableColumns::BintableColumns(fitsfile*& fptr, std::function<void(void)> touch, std::function<void(void)> edit) :
    m_fptr(fptr), m_touch(touch), m_edit(edit)
{}

Linx::Index BintableColumns::read_column_count() const
{
  m_touch();
  return Cfitsio::BintableIo::column_count(m_fptr);
}

Linx::Index BintableColumns::read_row_count() const
{
  m_touch();
  return Cfitsio::BintableIo::row_count(m_fptr);
}

Linx::Index BintableColumns::read_buffer_row_count() const
{
  Linx::Index size = 0;
  int status = 0;
  fits_get_rowsize(m_fptr, &size, &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot compute buffer row count.");
  return size;
}

bool BintableColumns::has(const std::string& name) const
{
  m_touch();
  return Cfitsio::BintableIo::has_column(m_fptr, name);
}

Linx::Index BintableColumns::read_index(const std::string& name) const
{
  m_touch();
  return Cfitsio::BintableIo::column_index(m_fptr, name) - 1;
}

std::vector<Linx::Index> BintableColumns::read_n_indices(const std::vector<std::string>& names) const
{
  m_touch();
  std::vector<Linx::Index> indices(names.size());
  std::transform(names.begin(), names.end(), indices.begin(), [&](const std::string& n) {
    return Cfitsio::BintableIo::column_index(m_fptr, n) - 1;
  });
  return indices;
}

std::string BintableColumns::read_name(Linx::Index index) const
{
  m_touch();
  return Cfitsio::BintableIo::column_name(m_fptr, index + 1);
}

std::vector<std::string> BintableColumns::read_all_names() const
{
  const auto size = read_column_count();
  std::vector<std::string> names(size);
  for (Linx::Index i = 0; i < size; ++i) {
    names[i] = read_name(i);
  }
  return names;
}

void BintableColumns::update_name(ColumnKey key, const std::string& name) const
{
  m_edit();
  Cfitsio::BintableIo::update_column_name(m_fptr, key.index(*this) + 1, name);
}

void BintableColumns::remove(ColumnKey key) const
{
  m_edit();
  int status = 0;
  fits_delete_col(m_fptr, key.index(*this) + 1, &status);
  Cfitsio::CfitsioError::may_throw(status, m_fptr, "Cannot remove column #" + std::to_string(key.index(*this)));
  // TODO to Cfitsio
}

void BintableColumns::remove_n(std::vector<ColumnKey> keys) const
{
  std::sort(keys.begin(), keys.end(), [&](auto& lhs, auto& rhs) {
    return lhs.index(*this) > rhs.index(*this); // descending order to avoid shifting
  });
  for (auto& k : keys) {
    remove(std::move(k)); // WARN Scans the whole HDU at every loop
  }
}

} // namespace Fits
