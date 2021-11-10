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

#if defined(_ELECFITSIOWRAPPER_HDUWRAPPER_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HduWrapper.h"

#include <utility> // index_sequence, make_index_sequence

namespace Euclid {
namespace Cfitsio {
namespace HduAccess {

template <typename T, long n>
void initImageExtension(fitsfile* fptr, const std::string& name, const Fits::Position<n>& shape) {
  mayThrowReadonlyError(fptr);
  int status = 0;
  auto nonconstShape = shape; // const-correctness issue
  fits_create_img(fptr, TypeCode<T>::bitpix(), n, &nonconstShape[0], &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create image extension: " + name);
  updateName(fptr, name);
}

template <typename T, long n>
void assignImageExtension(fitsfile* fptr, const std::string& name, const Fits::Raster<T, n>& raster) {
  initImageExtension<T, n>(fptr, name, raster.shape());
  ImageIo::writeRaster<T, n>(fptr, raster);
}

template <typename... Ts>
void initBintableExtension(fitsfile* fptr, const std::string& name, const Fits::ColumnInfo<Ts>&... infos) {
  constexpr long ncols = sizeof...(Ts);
  CStrArray colName {infos.name...};
  CStrArray colFormat {TypeCode<Ts>::tform(infos.repeatCount)...};
  CStrArray colUnit {infos.unit...};
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, ncols, colName.data(), colFormat.data(), colUnit.data(), name.c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension: " + name);
}

template <typename... Ts>
void assignBintableExtension(fitsfile* fptr, const std::string& name, const Fits::Column<Ts>&... columns) {
  initBintableExtension(fptr, name, columns.info()...);
  BintableIo::writeColumns(fptr, columns...);
}

/// @cond INTERNAL
namespace Internal {
template <typename Tuple, std::size_t... Is>
void assignBintableExtensionImpl(
    fitsfile* fptr,
    const std::string& name,
    const Tuple& columns,
    std::index_sequence<Is...>) {
  assignBintableExtension(fptr, name, std::get<Is>(columns)...);
}
} // namespace Internal
/// @endcond

template <typename Tuple, std::size_t size>
void assignBintableExtension(fitsfile* fptr, const std::string& name, const Tuple& columns) { // TODO tupleApply?
  Internal::assignBintableExtensionImpl<Tuple>(fptr, name, columns, std::make_index_sequence<size>());
}

template <typename T>
void assignBintableExtension(fitsfile* fptr, const std::string& name, const Fits::Column<T>& column) { // TODO used?
  constexpr long columnCount = 1;
  std::string colName = column.info().name;
  char* cName = &colName[0];
  std::string colFormat = TypeCode<T>::tform(column.info().repeatCount);
  char* cFormat = &colFormat[0];
  std::string colUnit = column.info().unit;
  char* cUnit = &colUnit[0];
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, columnCount, &cName, &cFormat, &cUnit, name.c_str(), &status);
  CfitsioError::mayThrow(status, fptr, "Cannot create binary table extension: " + name);
  BintableIo::writeColumn(fptr, column);
}

} // namespace HduAccess
} // namespace Cfitsio
} // namespace Euclid

#endif
