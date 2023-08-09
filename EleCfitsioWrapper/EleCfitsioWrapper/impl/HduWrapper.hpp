// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELECFITSIOWRAPPER_HDUWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleCfitsioWrapper/HduWrapper.h"
#include "EleCfitsioWrapper/ImageWrapper.h"

#include <utility> // index_sequence, make_index_sequence

namespace Euclid {
namespace Cfitsio {
namespace HduAccess {

template <typename T, long N>
void init_image(fitsfile* fptr, const std::string& name, const Fits::Position<N>& shape) {
  mayThrowReadonlyError(fptr);
  int status = 0;
  auto nonconstShape = shape; // const-correctness issue
  fits_create_img(fptr, TypeCode<T>::bitpix(), shape.size(), &nonconstShape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create image extension: " + name);
  update_name(fptr, name);
}

template <typename TRaster>
void assign_image(fitsfile* fptr, const std::string& name, const TRaster& raster) {
  init_image<typename TRaster::Value, TRaster::Dim>(fptr, name, raster.shape());
  ImageIo::writeRaster(fptr, raster);
}

template <typename... TInfos>
void init_bintable(fitsfile* fptr, const std::string& name, const TInfos&... infos) {
  constexpr long ncols = sizeof...(TInfos);
  Fits::String::CStrArray colName {infos.name...};
  Fits::String::CStrArray colFormat {TypeCode<typename TInfos::Value>::tform(infos.repeatCount())...};
  Fits::String::CStrArray colUnit {infos.unit...};
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, ncols, colName.data(), colFormat.data(), colUnit.data(), name.c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension: " + name);
  BintableIo::write_column_dims(fptr, 1, infos...);
}

template <typename... TColumns>
void assign_bintable(fitsfile* fptr, const std::string& name, const TColumns&... columns) {
  init_bintable(fptr, name, columns.info()...);
  BintableIo::write_columns(fptr, columns...);
}

/// @cond INTERNAL
namespace Internal {
template <typename TColumns, std::size_t... Is>
void assign_bintableImpl(
    fitsfile* fptr,
    const std::string& name,
    const TColumns& columns,
    std::index_sequence<Is...>) {
  assign_bintable(fptr, name, std::get<Is>(columns)...);
}
} // namespace Internal
/// @endcond

template <typename TTuple, std::size_t Size>
void assign_bintable(fitsfile* fptr, const std::string& name, const TTuple& columns) { // TODO tupleApply?
  Internal::assign_bintableImpl<TTuple>(fptr, name, columns, std::make_index_sequence<Size>());
}

template <typename TColumn>
void assign_bintable(fitsfile* fptr, const std::string& name, const TColumn& column) { // TODO used?
  constexpr long column_count = 1;
  std::string colName = column.info().name;
  char* cName = &colName[0];
  std::string colFormat = TypeCode<typename TColumn::Value>::tform(column.info().repeatCount());
  char* cFormat = &colFormat[0];
  std::string colUnit = column.info().unit;
  char* cUnit = &colUnit[0];
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, column_count, &cName, &cFormat, &cUnit, name.c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension: " + name);
  BintableIo::write_column(fptr, column);
}

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid

#endif
