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

#ifndef _ELECFITSIOWRAPPER_BINTABLEWRAPPER_H
#define _ELECFITSIOWRAPPER_BINTABLEWRAPPER_H

#include "EleCfitsioWrapper/CfitsioUtils.h"
#include "EleCfitsioWrapper/TypeWrapper.h"
#include "EleFitsData/Column.h"

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
template <typename T, long N = 1>
Fits::ColumnInfo<T, N> readColumnInfo(fitsfile* fptr, long index);

/**
 * @brief Read the binary table column with given index.
 */
template <typename T, long N = 1>
Fits::VecColumn<T, N> readColumn(fitsfile* fptr, long index);

/**
 * @brief Read the segment of a binary table column with given index.
 */
template <typename TColumn>
void readColumnSegment(fitsfile* fptr, const Fits::Segment& rows, long index, TColumn& column);

/**
 * @brief Read a binary table column with given name.
 */
template <typename T, long N = 1>
Fits::VecColumn<T, N> readColumn(fitsfile* fptr, const std::string& name);

/**
 * @brief Read several binary table columns with given indices.
 */
template <typename... Ts>
std::tuple<Fits::VecColumn<Ts, 1>...> readColumns(fitsfile* fptr, const std::vector<long>& indices);

/**
 * @brief Read several binary table columns with given names.
 */
template <typename... Ts>
std::tuple<Fits::VecColumn<Ts, 1>...> readColumns(fitsfile* fptr, const std::vector<std::string>& names);

/**
 * @brief Write a binary table column.
 */
template <typename TColumn>
void writeColumn(fitsfile* fptr, const TColumn& column);

/**
 * @brief Write a segment of a binary table column.
 */
template <typename TColumn>
void writeColumnSegment(fitsfile* fptr, long firstRow, const TColumn& column);

/**
 * @brief Write several binary table columns.
 */
template <typename... TColumns>
void writeColumns(fitsfile* fptr, const TColumns&... columns);

/**
 * @brief Insert a binary table column at given index.
 */
template <typename TColumn>
void insertColumn(fitsfile* fptr, long index, const TColumn& column);

/**
 * @brief Insert several binary table columns at given index.
 */
template <typename... TColumns>
void insertColumns(fitsfile* fptr, long index, const TColumns&... columns);

/**
 * @brief Append a binary table column.
 */
template <typename TColumn>
void appendColumn(fitsfile* fptr, const TColumn& column);

/**
 * @brief Append several binary table columns.
 */
template <typename... TColumns>
void appendColumns(fitsfile* fptr, const TColumns&... columns);

/**
 * @brief Read a segment of a column into some data pointer.
 */
template <typename T>
void readColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, T* data);

/**
 * @copydoc readColumnData()
 */
template <>
void readColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, std::string* data);

/**
 * @brief Write a segment of a column given by some data pointer.
 */
template <typename T>
void writeColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, const T* data);

/**
 * @copydoc writeColumnData()
 */
template <>
void writeColumnData(fitsfile* fptr, const Fits::Segment& rows, long index, long repeatCount, const std::string* data);

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/BintableWrapper.hpp"
#undef _ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
/// @endcond

#endif
