// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/CfitsioBenchmark.h"

#if defined(_ELEFITS_VALIDATION_CFITSIOBENCHMARK_IMPL) || defined(CHECK_QUALITY)

namespace Euclid {
namespace Fits {
namespace Validation {

template <std::size_t i>
void CfitsioBenchmark::setupColumnInfo(
    const BColumns& columns,
    std::vector<std::string>& names,
    std::vector<std::string>& formats,
    std::vector<std::string>& units) {
  const auto& col = std::get<i>(columns);
  names[i] = col.info().name;
  formats[i] = Cfitsio::TypeCode<typename std::decay_t<decltype(col)>::Value>::tform(col.info().repeatCount());
  units[i] = col.info().unit;
}

template <std::size_t i>
void CfitsioBenchmark::writeColumn(const BColumns& columns, long firstRow, long rowCount) {
  const auto& col = std::get<i>(columns);
  const auto b = col.vector().begin() + firstRow;
  const auto e = b + rowCount;
  using Value = typename std::decay_t<decltype(col)>::Value;
  std::vector<Value> nonconstVec(b, e);
  fits_write_col(
      m_fptr,
      Cfitsio::TypeCode<Value>::forBintable(),
      i + 1,
      firstRow + 1,
      1,
      rowCount,
      nonconstVec.data(),
      &m_status);
  mayThrow("Cannot write column");
}

template <std::size_t i>
void CfitsioBenchmark::initColumn(BColumns& columns, long rowCount) {
  auto& col = std::get<i>(columns);
  int colnum = i + 1;
  long repeat = 0;
  char ttype[FLEN_VALUE];
  char tunit[FLEN_VALUE];
  fits_get_bcolparms(m_fptr, colnum, ttype, tunit, nullptr, &repeat, nullptr, nullptr, nullptr, nullptr, &m_status);
  col = std::decay_t<decltype(col)>({ttype, tunit, repeat}, rowCount);
  mayThrow("Cannot initialize column");
}

template <std::size_t i>
void CfitsioBenchmark::readColumn(BColumns& columns, long firstRow, long rowCount) {
  auto& col = std::get<i>(columns);
  auto data = col.data() + firstRow;
  using Value = typename std::decay_t<decltype(col)>::Value;
  fits_read_col(
      m_fptr,
      Cfitsio::TypeCode<Value>::forBintable(),
      i + 1,
      firstRow + 1,
      1,
      rowCount,
      nullptr,
      data,
      nullptr,
      &m_status);
  mayThrow("Cannot read column");
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
