// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_HDUWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"

#include <utility> // index_sequence, make_index_sequence

namespace Cfitsio {
namespace HduAccess {

template <typename T, Linx::Index N>
void init_image(fitsfile* fptr, const std::string& name, const Linx::Position<N>& shape)
{
  may_throw_readonly(fptr);
  int status = 0;
  auto nonconst_shape = shape; // const-correctness issue
  fits_create_img(fptr, TypeCode<T>::bitpix(), shape.size(), &nonconst_shape[0], &status);
  CfitsioError::may_throw(status, fptr, "Cannot create image extension: " + name);
  update_name(fptr, name);
}

template <typename TRaster>
void assign_image(fitsfile* fptr, const std::string& name, const TRaster& raster)
{
  init_image<typename TRaster::Value, TRaster::Dimension>(fptr, name, raster.shape());
  ImageIo::write_raster(fptr, raster);
}

template <typename... TInfos>
void init_bintable(fitsfile* fptr, const std::string& name, const TInfos&... infos)
{
  constexpr Linx::Index ncols = sizeof...(TInfos);
  Fits::String::CStrArray col_name {infos.name...};
  Fits::String::CStrArray col_format {TypeCode<typename TInfos::Value>::tform(infos.repeat_count())...};
  Fits::String::CStrArray col_unit {infos.unit...};
  int status = 0;
  fits_create_tbl(
      fptr,
      BINARY_TBL,
      0,
      ncols,
      col_name.data(),
      col_format.data(),
      col_unit.data(),
      name.c_str(),
      &status);
  CfitsioError::may_throw(status, fptr, "Cannot create binary table extension: " + name);
  BintableIo::write_column_dims(fptr, 1, infos...);
}

template <typename... TColumns>
void assign_bintable(fitsfile* fptr, const std::string& name, const TColumns&... columns)
{
  init_bintable(fptr, name, columns.info()...);
  BintableIo::write_columns(fptr, columns...);
}

/// @cond INTERNAL
namespace Internal {
template <typename TColumns, std::size_t... Is>
void assign_bintableImpl(fitsfile* fptr, const std::string& name, const TColumns& columns, std::index_sequence<Is...>)
{
  assign_bintable(fptr, name, std::get<Is>(columns)...);
}
} // namespace Internal
/// @endcond

template <typename TTuple, std::size_t Size>
void assign_bintable(fitsfile* fptr, const std::string& name, const TTuple& columns)
{ // TODO tuple_apply?
  Internal::assign_bintableImpl<TTuple>(fptr, name, columns, std::make_index_sequence<Size>());
}

template <typename TColumn>
void assign_bintable(fitsfile* fptr, const std::string& name, const TColumn& column)
{ // TODO used?
  constexpr Linx::Index column_count = 1;
  std::string col_name = column.info().name;
  char* c_name = &col_name[0];
  std::string col_format = TypeCode<typename TColumn::Value>::tform(column.info().repeat_count());
  char* c_format = &col_format[0];
  std::string col_unit = column.info().unit;
  char* c_unit = &col_unit[0];
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, column_count, &c_name, &c_format, &c_unit, name.c_str(), &status);
  CfitsioError::may_throw(status, fptr, "Cannot create binary table extension: " + name);
  BintableIo::write_column(fptr, column);
}

} // namespace HduAccess
} // namespace Cfitsio

#endif
