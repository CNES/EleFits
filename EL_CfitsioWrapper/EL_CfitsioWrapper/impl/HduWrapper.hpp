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

#include "EL_CfitsioWrapper/HduWrapper.h"

namespace Euclid {
namespace Cfitsio {
namespace Hdu {

template <typename T, long n>
void createImageExtension(fitsfile *fptr, const std::string &name, const FitsIO::Position<n> &shape) {
  mayThrowReadonlyError(fptr);
  int status = 0;
  auto nonconstShape = shape; // const-correctness issue
  fits_create_img(fptr, TypeCode<T>::bitpix(), n, &nonconstShape[0], &status);
  mayThrowCfitsioError(status, "Cannot create image extension");
  updateName(fptr, name);
}

template <typename T, long n>
void createImageExtension(fitsfile *fptr, const std::string &name, const FitsIO::Raster<T, n> &raster) {
  mayThrowReadonlyError(fptr);
  createImageExtension<T, n>(fptr, name, raster.shape);
  Image::writeRaster<T, n>(fptr, raster);
}

template <typename... Ts>
void createBintableExtension(fitsfile *fptr, const std::string &name, const FitsIO::ColumnInfo<Ts> &... header) {
  constexpr long ncols = sizeof...(Ts);
  CStrArray colName { header.name... };
  CStrArray colFormat { TypeCode<Ts>::tform(header.repeat)... };
  CStrArray colUnit { header.unit... };
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, ncols, colName.data(), colFormat.data(), colUnit.data(), name.c_str(), &status);
  mayThrowCfitsioError(status, "Cannot create bintable extension " + name);
}

template <typename... Ts>
void createBintableExtension(fitsfile *fptr, const std::string &name, const FitsIO::Column<Ts> &... table) {
  constexpr long ncols = sizeof...(Ts);
  CStrArray colName { table.info.name... };
  CStrArray colFormat { TypeCode<Ts>::tform(table.info.repeat)... };
  CStrArray colUnit { table.info.unit... };
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, ncols, colName.data(), colFormat.data(), colUnit.data(), name.c_str(), &status);
  mayThrowCfitsioError(status, "Cannot create bintable extension " + name);
  Bintable::writeColumns(fptr, table...);
}

template <typename T>
void createBintableExtension(fitsfile *fptr, const std::string &name, const FitsIO::Column<T> &column) {
  constexpr long count = 1;
  std::string colName = column.info.name;
  char *cName = &colName[0];
  std::string colFormat = TypeCode<T>::tform(column.info.repeat);
  char *cFormat = &colFormat[0];
  std::string colUnit = column.info.unit;
  char *cUnit = &colUnit[0];
  int status = 0;
  fits_create_tbl(fptr, BINARY_TBL, 0, count, &cName, &cFormat, &cUnit, name.c_str(), &status);
  mayThrowCfitsioError(status, "Cannot create bintable extension " + name);
  Bintable::writeColumn(fptr, column);
}

} // namespace Hdu
} // namespace Cfitsio
} // namespace Euclid
