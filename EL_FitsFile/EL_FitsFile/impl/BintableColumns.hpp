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

#if defined(_EL_FITSFILE_BINTABLECOLUMNS_IMPL) || defined(CHECK_QUALITY)

  #include "EL_FitsFile/BintableColumns.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
VecColumn<T> read(const std::string& name) const {
  m_touch();
  Cfitsio::Bintable::readColumn<T>(name);
}

template <typename T>
VecColumn<T> read(long index) const {
  m_touch();
  Cfitsio::Bintable::readColumn<T>(name);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> readSeq(const Named<Ts>&... names) const {
  m_touch();
  Cfitsio::Bintable::readColumns<Ts...>(names.name...);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> readSeq(const Indexed<Ts>&... indices) const {
  m_touch();
  Cfitsio::Bintable::readColumns<Ts...>(indices.index...);
}

template <typename T>
void write(const Column<T>& column) const {
  m_edit();
  Cfitsio::Bintable::writeColumn(column);
}

template <typename T>
void insert(const Column<T>& column, long index = -1) const {
  m_edit();
  if (index == -1) {
    Cfitsio::Bintable::appendColumn(column); // FIXME handle other values
  }
  Cfitsio::Bintable::insertColumn(m_fptr, index, column);
}

template <typename... Ts>
void writeSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::writeColumns(m_fptr, columns);
}

template <typename TSeq>
void writeSeq(TSeq&& columns) const {
  // FIXME implement!
}

template <typename... Ts>
void appendSeq(const Column<Ts>&... columns) const {
  m_edit();
  Cfitsio::Bintable::appendColumns(m_fptr, columns);
}

} // namespace FitsIO
} // namespace Euclid

#endif
