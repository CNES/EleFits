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

#ifndef _EL_CFITSIOWRAPPER_HDUWRAPPER_H
#define _EL_CFITSIOWRAPPER_HDUWRAPPER_H

#include <fitsio.h>
#include <string>

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/ImageWrapper.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"

namespace Euclid {
namespace Cfitsio {

/**
 * @brief HDU-related functions.
 *
 * @details
 * An HDU can be of three Types (ASCII tables are not supported):
 * * Image
 * * Bintable
 *
 * Getter functions generally apply to the current HDU.
 * Functions to go to an HDU return false if target HDU is already current HDU.
 * Functions to create an HDU append it at the end of the file.
 */
namespace Hdu {

/**
 * @brief HDU type (ASCII tables not supported).
 */
enum class Type {
  Image, ///< Image HDU
  Bintable ///< Binary table HDU
};

/**
 * @brief Read the number of HDUs in a Fits file.
 * @warning Empty or incomplete HDUs are not counted.
 */
long count(fitsfile *fptr);

/**
 * @brief Get the index of the current HDU.
 */
long currentIndex(fitsfile *fptr);

/**
 * @brief Get the name of the current HDU.
 */
std::string currentName(fitsfile *fptr);

/**
 * @brief Get the Type of the current HDU.
 */
Type currentType(fitsfile *fptr);

/**
 * @brief Check whether current HDU is the Primary HDU.
 */
bool currentIsPrimary(fitsfile *fptr);

/**
 * @brief Go to an HDU specified by its index.
 */
bool gotoIndex(fitsfile *fptr, long index);

/**
 * @brief Go to an HDU specified by its name.
 */
bool gotoName(fitsfile *fptr, const std::string &name);

/**
 * @brief Go to an HDU specified by incrementing the index by a given amount.
 */
bool gotoNext(fitsfile *fptr, long step = 1);

/**
 * @brief Go to the Primary HDU.
 */
bool gotoPrimary(fitsfile *fptr);

/**
 * @brief Initialize the Primary HDU if not done.
 */
bool initPrimary(fitsfile *fptr);

/**
 * @brief Write or update HDU name.
 */
bool updateName(fitsfile *fptr, const std::string &name);

/**
 * @brief Create a HDU of Type METADATA.
 */
void createMetadataExtension(fitsfile *fptr, const std::string &name);

/**
 * @brief Create a new Image HDU with given name, pixel type and shape.
 */
template <typename T, long n = 2>
void createImageExtension(fitsfile *fptr, const std::string &name, const FitsIO::Position<n> &shape);

/**
 * @brief Write a Raster in a new Image HDU.
 */
template <typename T, long n = 2>
void createImageExtension(fitsfile *fptr, const std::string &name, const FitsIO::Raster<T, n> &raster);

/**
 * @brief Create a new Bintable HDU with given name and column infos.
 */
template <typename... Ts>
void createBintableExtension(fitsfile *fptr, const std::string &name, const FitsIO::ColumnInfo<Ts> &... header);

/**
 * @brief Write a Table in a new Bintable HDU.
 */
template <typename... Ts>
void createBintableExtension(fitsfile *fptr, const std::string &name, const FitsIO::Column<Ts> &... table);

/**
 * @brief Delete the HDU at given index.
 */
void deleteHdu(fitsfile *fptr, long index);

/////////////////////
// IMPLEMENTATION //
///////////////////

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

#endif
