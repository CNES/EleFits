// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/BintableHdu.h"

namespace Fits {

BintableHdu::BintableHdu(Token token, fitsfile*& fptr, Linx::Index index, HduCategory status) :
    Hdu(token, fptr, index, HduCategory::Bintable, status),
    m_columns(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        })
{}

BintableHdu::BintableHdu() :
    Hdu(),
    m_columns(
        m_fptr,
        [&]() {
          touch();
        },
        [&]() {
          edit();
        })
{}

const BintableColumns& BintableHdu::columns() const
{
  return m_columns;
}

Linx::Index BintableHdu::read_column_count() const
{
  touch();
  return Cfitsio::BintableIo::column_count(m_fptr);
}

Linx::Index BintableHdu::read_row_count() const
{
  touch();
  return Cfitsio::BintableIo::row_count(m_fptr);
}

HduCategory BintableHdu::category() const
{
  auto cat = Hdu::category();
  if (read_column_count() == 0 || read_row_count() == 0) {
    cat &= HduCategory::Metadata;
  } else {
    cat &= HduCategory::Data;
  }
  return cat;
}

template <>
const BintableColumns& Hdu::as() const
{
  return as<BintableHdu>().columns();
}

#ifndef COMPILE_READ_COLUMN
#define COMPILE_READ_COLUMN(T, _) template VecColumn<T, 1> BintableHdu::read_column(ColumnKey) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_READ_COLUMN)
#undef COMPILE_READ_COLUMN
#endif

#ifndef COMPILE_WRITE_COLUMN
#define COMPILE_WRITE_COLUMN(T, _) \
  template void BintableHdu::write_column(const PtrColumn<T, 1>&) const; \
  template void BintableHdu::write_column(const PtrColumn<const T, 1>&) const; \
  template void BintableHdu::write_column(const VecColumn<T, 1>&) const;
ELEFITS_FOREACH_COLUMN_TYPE(COMPILE_WRITE_COLUMN)
#undef COMPILE_WRITE_COLUMN
#endif

} // namespace Fits
