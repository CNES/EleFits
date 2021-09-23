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

#ifndef _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H
#define _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_H

#include "EL_CfitsioWrapper/CfitsioUtils.h"
#include "EL_CfitsioWrapper/TypeWrapper.h"
#include "EL_FitsData/Column.h"

#include <tuple>
#include <vector>

namespace Euclid {
namespace Cfitsio {

/**
 * @brief Binary table-related functions.
 */
namespace BintableIo {

/**
 * @brief Get the number of columns.
 */
long columnCount(fitsfile* fptr);

/**
 * @brief Get the number of rows.
 */
long rowCount(fitsfile* fptr);

/**
 * @brief Check whether a given column exists.
 */
bool hasColumn(fitsfile* fptr, const std::string& name);

/**
 * @brief Get the name of a given column.
 */
std::string columnName(fitsfile* fptr, long index);

/**
 * @brief Update the name of a given column.
 */
void updateColumnName(fitsfile* fptr, long index, const std::string& newName);

/**
 * @brief Get the index of a binary table column.
 */
long columnIndex(fitsfile* fptr, const std::string& name);

/**
 * @brief Read the metadata of a binary table column with given index.
 */
template <typename T>
FitsIO::ColumnInfo<T> readColumnInfo(fitsfile* fptr, long index);

/**
 * @brief Read the binary table column with given index.
 */
template <typename T>
FitsIO::VecColumn<T> readColumn(fitsfile* fptr, long index);

/**
 * @brief Read the segment of a binary table column with given index.
 */
template <typename T>
void readColumnSegment(fitsfile* fptr, const FitsIO::Segment& rows, long index, FitsIO::Column<T>& column);

/**
 * @brief Read a binary table column with given name.
 */
template <typename T>
FitsIO::VecColumn<T> readColumn(fitsfile* fptr, const std::string& name);

/**
 * @brief Read several binary table columns with given indices.
 */
template <typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> readColumns(fitsfile* fptr, const std::vector<long>& indices);

/**
 * @brief Read several binary table columns with given names.
 */
template <typename... Ts>
std::tuple<FitsIO::VecColumn<Ts>...> readColumns(fitsfile* fptr, const std::vector<std::string>& names);

/**
 * @brief Write a binary table column.
 */
template <typename T>
void writeColumn(fitsfile* fptr, const FitsIO::Column<T>& column);

/**
 * @brief Write a segment of a binary table column.
 */
template <typename T>
void writeColumnSegment(fitsfile* fptr, long firstRow, const FitsIO::Column<T>& column);

/**
 * @brief Write several binary table columns.
 */
template <typename... Ts>
void writeColumns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns);

/**
 * @brief Insert a binary table column at given index.
 */
template <typename T>
void insertColumn(fitsfile* fptr, long index, const FitsIO::Column<T>& column);

/**
 * @brief Insert several binary table columns at given index.
 */
template <typename... Ts>
void insertColumns(fitsfile* fptr, long index, const FitsIO::Column<Ts>&... columns);

/**
 * @brief Append a binary table column.
 */
template <typename T>
void appendColumn(fitsfile* fptr, const FitsIO::Column<T>& column);

/**
 * @brief Append several binary table columns.
 */
template <typename... Ts>
void appendColumns(fitsfile* fptr, const FitsIO::Column<Ts>&... columns);

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
#include "EL_CfitsioWrapper/impl/BintableWrapper.hpp"
#undef _EL_CFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
/// @endcond

#endif
