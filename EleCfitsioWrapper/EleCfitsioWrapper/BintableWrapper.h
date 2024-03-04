// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_BINTABLEWRAPPER_H
#define _ELECFITSIOWRAPPER_BINTABLEWRAPPER_H

#include "EleFitsData/Column.h"

#include <fitsio.h>
#include <string>
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
long column_count(fitsfile* fptr);

/**
 * @brief Get the number of rows.
 */
long row_count(fitsfile* fptr);

/**
 * @brief Check whether a given column exists.
 */
bool has_column(fitsfile* fptr, const std::string& name);

/**
 * @brief Get the name of a given column.
 */
std::string column_name(fitsfile* fptr, long index);

/**
 * @brief Update the name of a given column.
 */
void update_column_name(fitsfile* fptr, long index, const std::string& new_name);

/**
 * @brief Get the index of a binary table column.
 */
long column_index(fitsfile* fptr, const std::string& name);

/**
 * @brief Read the metadata of a binary table column with given index.
 */
template <typename T, long N = 1>
Fits::ColumnInfo<T, N> read_column_info(fitsfile* fptr, long index);

/**
 * @brief Read the TDIM keyword (if any).
 */
template <long N>
void read_column_dim(fitsfile* fptr, long index, Linx::Position<N>& info);

/**
 * @brief Read the binary table column with given index.
 */
template <typename T, long N = 1>
Fits::VecColumn<T, N> read_column(fitsfile* fptr, long index);

/**
 * @brief Read the segment of a binary table column with given index.
 */
template <typename TColumn>
void read_column_segment(fitsfile* fptr, const Fits::Segment& rows, long index, TColumn& column);

/**
 * @brief Read a binary table column with given name.
 */
template <typename T, long N = 1>
Fits::VecColumn<T, N> read_column(fitsfile* fptr, const std::string& name);

/**
 * @brief Read several binary table columns with given indices.
 */
template <typename... Ts>
std::tuple<Fits::VecColumn<Ts, 1>...> read_columns(fitsfile* fptr, const std::vector<long>& indices);

/**
 * @brief Read several binary table columns with given names.
 */
template <typename... Ts>
std::tuple<Fits::VecColumn<Ts, 1>...> read_columns(fitsfile* fptr, const std::vector<std::string>& names);

/**
 * @brief Write a binary table column.
 */
template <typename TColumn>
void write_column(fitsfile* fptr, const TColumn& column);

/**
 * @brief Write the TDIM keyword if needed.
 * @details
 * The keyword will be written only if the dimension is > 1
 * or if the shape does not correspond to the repeat count.
 */
template <long N>
void write_column_dim(fitsfile* fptr, long index, const Linx::Position<N>& shape);

/**
 * @brief Write several TDIM keywords if needed.
 */
template <typename... TInfos>
void write_column_dims(fitsfile* fptr, long index, const TInfos&... infos);

/**
 * @brief Write a segment of a binary table column.
 */
template <typename TColumn>
void write_column_segment(fitsfile* fptr, long first_row, const TColumn& column);

/**
 * @brief Write several binary table columns.
 */
template <typename... TColumns>
void write_columns(fitsfile* fptr, const TColumns&... columns);

/**
 * @brief Insert a binary table column at given index.
 */
template <typename TColumn>
void insert_column(fitsfile* fptr, long index, const TColumn& column);

/**
 * @brief Insert several binary table columns at given index.
 */
template <typename... TColumns>
void insert_columns(fitsfile* fptr, long index, const TColumns&... columns);

/**
 * @brief Append a binary table column.
 */
template <typename TColumn>
void append_column(fitsfile* fptr, const TColumn& column);

/**
 * @brief Append several binary table columns.
 */
template <typename... TColumns>
void append_columns(fitsfile* fptr, const TColumns&... columns);

/**
 * @brief Read a segment of a column into some data pointer.
 */
template <typename T>
void read_column_data(fitsfile* fptr, const Fits::Segment& rows, long index, long repeat_count, T* data);

/**
 * @copydoc read_column_data()
 */
template <>
void read_column_data(fitsfile* fptr, const Fits::Segment& rows, long index, long repeat_count, std::string* data);

/**
 * @brief Write a segment of a column given by some data pointer.
 */
template <typename T>
void write_column_data(fitsfile* fptr, const Fits::Segment& rows, long index, long repeat_count, const T* data);

/**
 * @copydoc write_column_data()
 */
template <>
void write_column_data(
    fitsfile* fptr,
    const Fits::Segment& rows,
    long index,
    long repeat_count,
    const std::string* data);

} // namespace BintableIo
} // namespace Cfitsio
} // namespace Euclid

/// @cond INTERNAL
#define _ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
#include "EleCfitsioWrapper/impl/BintableWrapper.hpp"
#undef _ELECFITSIOWRAPPER_BINTABLEWRAPPER_IMPL
/// @endcond

#endif
