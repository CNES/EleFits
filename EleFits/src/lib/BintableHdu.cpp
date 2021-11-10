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

#include "EleFits/BintableHdu.h"

namespace Euclid {
namespace Fits {

BintableHdu::BintableHdu(Token token, fitsfile*& fptr, long index, HduCategory status) :
    Hdu(token, fptr, index, HduCategory::Bintable, status),
    m_columns(
        m_fptr,
        [&]() {
          touchThisHdu();
        },
        [&]() {
          editThisHdu();
        }) {}

BintableHdu::BintableHdu() :
    Hdu(),
    m_columns(
        m_fptr,
        [&]() {
          touchThisHdu();
        },
        [&]() {
          editThisHdu();
        }) {}

const BintableColumns& BintableHdu::columns() const {
  return m_columns;
}

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

#ifndef COMPILE_READ_COLUMN
#define COMPILE_READ_COLUMN(type, unused) template VecColumn<type> BintableHdu::readColumn(const std::string&) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_READ_COLUMN)
#undef COMPILE_READ_COLUMN
#endif

#ifndef COMPILE_WRITE_COLUMN
#define COMPILE_WRITE_COLUMN(type, unused) template void BintableHdu::writeColumn(const Column<type>&) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_WRITE_COLUMN)
#undef COMPILE_WRITE_COLUMN
#endif

} // namespace Fits
} // namespace Euclid
