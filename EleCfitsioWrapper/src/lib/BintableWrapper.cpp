// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/BintableWrapper.h"

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsUtils/StringUtils.h"

#include <algorithm>

namespace Cfitsio {
namespace BintableIo {

Linx::Index column_count(fitsfile* fptr)
{
  int status = 0;
  int ncols = 0;
  fits_get_num_cols(fptr, &ncols, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read the number of columns");
  return ncols;
}

Linx::Index row_count(fitsfile* fptr)
{
  int status = 0;
  Linx::Index nrows = 0;
  fits_get_num_rows(fptr, &nrows, &status);
  CfitsioError::may_throw(status, fptr, "Cannot read the number of rows");
  return nrows;
}

bool has_column(fitsfile* fptr, const std::string& name)
{
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, Fits::String::to_char_ptr(name).get(), &index, &status);
  return (status == 0) || (status == COL_NOT_UNIQUE);
}

std::string column_name(fitsfile* fptr, Linx::Index index)
{
  int status = 0;
  char ttype[FLEN_VALUE];
  fits_get_bcolparms(
      fptr,
      index, // 1-based here
      ttype,
      nullptr, // tunit
      nullptr, // dtype
      nullptr, // repeat
      nullptr, // tscal
      nullptr, // tzero
      nullptr, // tnull
      nullptr, // tdisp
      &status);
  // TODO Should we just read TTYPEn instead ?
  CfitsioError::may_throw(status, fptr, "Cannot find name of column: " + std::to_string(index - 1));
  return ttype;
}

void update_column_name(fitsfile* fptr, Linx::Index index, const std::string& new_name)
{
  const std::string keyword = "TTYPE" + std::to_string(index);
  Cfitsio::HeaderIo::update_record<std::string>(fptr, {keyword, new_name});
  int status = 0;
  fits_set_hdustruc(fptr, &status); // Update internal fptr state to take into account new value
  // FIXME fits_set_hdustruc is deprecated => ask CFITSIO support
  CfitsioError::may_throw(status, fptr, "Cannot update name of column #" + std::to_string(index - 1));
}

Linx::Index column_index(fitsfile* fptr, const std::string& name)
{
  int index = 0;
  int status = 0;
  fits_get_colnum(fptr, CASESEN, Fits::String::to_char_ptr(name).get(), &index, &status);
  CfitsioError::may_throw(status, fptr, "Cannot find index of column: " + name);
  return index;
}

template <>
void read_column_dim(fitsfile* fptr, Linx::Index index, Linx::Position<-1>& shape)
{
  if (not HeaderIo::has_keyword(fptr, std::string("TDIM") + std::to_string(index))) {
    return;
  }
  int status = 0;
  std::vector<Linx::Index> naxes(999); // Max allowed number of axes
  int naxis = 0;
  fits_read_tdim(fptr, static_cast<int>(index), 999, &naxis, shape.data(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot read column dimension: #" + std::to_string(index - 1));
  naxes.resize(naxis);
  shape = Linx::Position<-1>(LINX_MOVE(naxes));
}

template <>
void read_column_data(
    fitsfile* fptr,
    const Fits::Segment& rows,
    Linx::Index index,
    Linx::Index repeat_count,
    std::string* data)
{
  int status = 0;
  std::vector<char*> vec(rows.size());
  std::generate(vec.begin(), vec.end(), [&]() {
    return (char*)malloc(repeat_count);
  });
  fits_read_col(
      fptr,
      TypeCode<std::string>::for_bintable(),
      static_cast<int>(index), // Column indices are int
      rows.front,
      1,
      rows.size(),
      nullptr,
      vec.data(),
      nullptr,
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot read column data: #" + std::to_string(index - 1));
  auto column_it = data;
  for (auto vec_it = vec.begin(); vec_it != vec.end(); ++vec_it, ++column_it) {
    *column_it = std::string(*vec_it);
    free(*vec_it);
  }
}

template <>
void write_column_data(
    fitsfile* fptr,
    const Fits::Segment& rows,
    Linx::Index index,
    Linx::Index,
    const std::string* data)
{
  int status = 0;
  Fits::String::CStrArray array(data, data + rows.size());
  fits_write_col(
      fptr,
      TypeCode<std::string>::for_bintable(),
      static_cast<int>(index), // column indices are int
      rows.front,
      1,
      rows.size(),
      array.data(),
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot write column data: #" + std::to_string(index - 1));
}

} // namespace BintableIo
} // namespace Cfitsio
