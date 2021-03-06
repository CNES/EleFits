// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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

template <>
const BintableColumns& Hdu::as() const {
  return as<BintableHdu>().columns();
}

#ifndef COMPILE_READ_COLUMN
#define COMPILE_READ_COLUMN(T, _) template VecColumn<T, 1> BintableHdu::readColumn(ColumnKey) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_READ_COLUMN)
#undef COMPILE_READ_COLUMN
#endif

#ifndef COMPILE_WRITE_COLUMN
#define COMPILE_WRITE_COLUMN(T, _) \
  template void BintableHdu::writeColumn(const PtrColumn<T, 1>&) const; \
  template void BintableHdu::writeColumn(const PtrColumn<const T, 1>&) const; \
  template void BintableHdu::writeColumn(const VecColumn<T, 1>&) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_WRITE_COLUMN)
#undef COMPILE_WRITE_COLUMN
#endif

} // namespace Fits
} // namespace Euclid
