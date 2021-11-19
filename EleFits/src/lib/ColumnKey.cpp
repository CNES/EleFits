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

#include "EleFits/ColumnKey.h"

#include "EleFits/BintableColumns.h"

namespace Euclid {
namespace Fits {

ColumnKey::ColumnKey(long index) : m_index(index), m_name() {}

ColumnKey::ColumnKey(int index) : ColumnKey(static_cast<long>(index)) {}

ColumnKey::ColumnKey(const std::string& name) : m_index(), m_name(name) {}

ColumnKey::ColumnKey(const char* name) : ColumnKey(std::string(name)) {}

long ColumnKey::index(const BintableColumns& columns) {
  if (m_index) {
    if (*m_index < 0) {
      *m_index += columns.readColumnCount(); // FIXME check range
    }
  } else {
    m_index = columns.readIndex(*m_name);
  }
  return *m_index;
}

const std::string& ColumnKey::name(const BintableColumns& columns) {
  if (not m_name) {
    m_name = columns.readName(*m_index);
  }
  return *m_name;
}

} // namespace Fits
} // namespace Euclid