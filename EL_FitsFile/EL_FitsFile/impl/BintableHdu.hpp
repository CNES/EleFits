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

#if defined(_EL_FITSFILE_BINTABLEHDU_IMPL) || defined(CHECK_QUALITY)

#include <algorithm>

#include "EL_FitsFile/BintableHdu.h"

namespace Euclid {
namespace FitsIO {

template <typename T>
VecColumn<T> BintableHdu::readColumn(long index) const {
  gotoThisHdu();
  return Cfitsio::Bintable::readColumn<T>(m_fptr, index + 1);
}

template <typename T>
VecColumn<T> BintableHdu::readColumn(const std::string& name) const {
  gotoThisHdu();
  return Cfitsio::Bintable::readColumn<T>(m_fptr, name);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::readColumns(const std::vector<long>& indices) const {
  gotoThisHdu();
  std::vector<long> cfitsioIndices(indices.size());
  std::transform(indices.begin(), indices.end(), cfitsioIndices.begin(), [](long i) {
    return i + 1;
  });
  return Cfitsio::Bintable::readColumns<Ts...>(m_fptr, cfitsioIndices);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::readColumns(const Indexed<Ts>&... indices) const {
  return readColumns<Ts...>(std::vector<long> { indices.index... });
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::readColumns(const std::vector<std::string>& names) const {
  gotoThisHdu();
  return Cfitsio::Bintable::readColumns<Ts...>(m_fptr, names);
}

template <typename... Ts>
std::tuple<VecColumn<Ts>...> BintableHdu::readColumns(const Named<Ts>&... names) const {
  return readColumns<Ts...>(std::vector<std::string> { names.name... });
}

template <typename T>
void BintableHdu::writeColumn(const Column<T>& column) const {
  gotoThisHdu();
  Cfitsio::Bintable::writeColumn(m_fptr, column);
}

template <typename... Ts>
void BintableHdu::writeColumns(const Column<Ts>&... columns) const {
  gotoThisHdu();
  Cfitsio::Bintable::writeColumns(m_fptr, columns...);
}

template <typename T>
void BintableHdu::appendColumn(const Column<T>& column) const {
  gotoThisHdu();
  Cfitsio::Bintable::appendColumn(m_fptr, column);
}

template <typename... Ts>
void BintableHdu::appendColumns(const Column<Ts>&... columns) const {
  gotoThisHdu();
  Cfitsio::Bintable::appendColumns(m_fptr, columns...);
}

#ifndef DECLARE_READ_COLUMN
#define DECLARE_READ_COLUMN(type, unused) \
  extern template VecColumn<type> BintableHdu::readColumn(const std::string&) const;
EL_FITSIO_FOREACH_COLUMN_TYPE(DECLARE_READ_COLUMN)
#undef DECLARE_READ_COLUMN
#endif

#ifndef DECLARE_WRITE_COLUMN
#define DECLARE_WRITE_COLUMN(type, unused) extern template void BintableHdu::writeColumn(const Column<type>&) const;
EL_FITSIO_FOREACH_COLUMN_TYPE(DECLARE_WRITE_COLUMN)
#undef DECLARE_WRITE_COLUMN
#endif

} // namespace FitsIO
} // namespace Euclid

#endif
