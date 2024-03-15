// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/ColumnKey.h"

#include "EleFits/BintableColumns.h"

namespace Fits {

ColumnKey::ColumnKey(Linx::Index index) : m_index(index), m_name() {}

ColumnKey::ColumnKey(int index) : ColumnKey(static_cast<Linx::Index>(index)) {}

ColumnKey::ColumnKey(const std::string& name) : m_index(), m_name(name) {}

ColumnKey::ColumnKey(const char* name) : ColumnKey(std::string(name)) {}

Linx::Index ColumnKey::index(const BintableColumns& columns)
{
  if (m_index) {
    if (*m_index < 0) {
      *m_index += columns.read_column_count(); // FIXME check range
    }
  } else {
    m_index = columns.read_index(*m_name);
  }
  return *m_index;
}

const std::string& ColumnKey::name(const BintableColumns& columns)
{
  if (not m_name) {
    m_name = columns.read_name(*m_index);
  }
  return *m_name;
}

} // namespace Fits
