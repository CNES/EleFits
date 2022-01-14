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

#ifndef _ELEFITSDATA_COLUMNINFO_H
#define _ELEFITSDATA_COLUMNINFO_H

#include "EleFitsData/Position.h"

#include <string>

namespace Euclid {
namespace Fits {

/**
 * @ingroup bintable_data_classes
 * @brief Loop over supported column types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EleFitsPrintSupportedTypes to display all supported types.
 * @see ELEFITS_FOREACH_RECORD_TYPE
 * @see ELEFITS_FOREACH_RASTER_TYPE
 */
#define ELEFITS_FOREACH_COLUMN_TYPE(MACRO) \
  /* MACRO(bool, bool) // TODO Could be supported at some point */ \
  MACRO(char, char) \
  MACRO(std::int16_t, int16) \
  MACRO(std::int32_t, int32) \
  MACRO(std::int64_t, int64) \
  MACRO(float, float) \
  MACRO(double, double) \
  MACRO(std::complex<float>, complex_float) \
  MACRO(std::complex<double>, complex_double) \
  MACRO(std::string, string) \
  MACRO(unsigned char, uchar) \
  MACRO(std::uint16_t, uint16) \
  MACRO(std::uint32_t, uint32) \
  MACRO(std::uint64_t, uint64)

/**
 * @ingroup bintable_data_classes
 * @brief Column metadata, i.e. name, unit, entry shape and value type.
 * @details
 * Binary tables can be seen as a sequence of columns or rows, made of consecutive entries.
 * Entry values are not necessarily simple types:
 * they can be either string, scalar, vector or multidimensional.
 * Yet, all the entries of one column have the same properties.
 * 
 * Here is a table which summarizes the properties of each column type
 * -- and more details follow.
 * 
 * <table class="fieldtable">
 * <tr><th>Type<th>`T`<th>`N`<th>`repeatCount`<th>`elementCountPerEntry`
 * <tr><td>String<td>`std::string`<td>1<td>> max number of characters<td>1
 * <tr><td>Scalar<td>Not `std::string`<td>1<td>1<td>1
 * <tr><td>Vector<td>Not `std::string`<td>1<td>> 1<td>= `repeatCount`
 * <tr><td>Multidimensional<td>Not `std::string`<td>-1 or > 1<td>Unconstrained<td>= `repeatCount`
 * </table>
 * 
 * In the case of vector and multidimensional columns, each entry contains `repeatCount` values.
 * Here is an example of a 4-row table with a scalar column and a vector column with a repeat count of 3:
 * <table class="fieldtable">
 * <tr><th>Row<th>`repeatCount` = 1<th>`repeatCount` = 3
 * <tr><td>0<td>00<td>00, 01, 02
 * <tr><td>1<td>10<td>10, 11, 12
 * <tr><td>2<td>20<td>20, 21, 22
 * <tr><td>3<td>30<td>30, 31, 32
 * </table>
 * For performance, the values are stored sequentially in a 1D array as follows:
 * \code
 * int repeat1[] = {00, 10, 20, 30};
 * int repeat3[] = {00, 01, 02, 10, 11, 12, 20, 21, 22, 30, 31, 32};
 * \endcode
 *
 * The only exception to this is string columns, which are like vector columns
 * -- they should have a repeat count greater than the maximum number of characters in a cell --
 * but each cell contains only one string:
 * <table class="fieldtable">
 * <tr><th>Row<th>`repeatCount` = 6
 * <tr><td>0<td>`"ZERO"`
 * <tr><td>1<td>`"ONE"`
 * <tr><td>2<td>`"TWO"`
 * <tr><td>3<td>`"THREE"`
 * </table>
 * 
 * The data array is a simple array of `std::string`s:
 * \code
 * std::string data[] = {"ZERO", "ONE", "TWO", "THREE"};
 * \endcode
 * but the repeat count should be at least 6 (beware of the null terminating character).
 * 
 * For convenience, we distinguish:
 * - repeat count, which is defined by the Fits standard;
 * - element count, which is the number of elements of type `T` in the container;
 * - element count per row, which is the element count divided by the row count.
 * 
 * Therefore, for `T = std::string`, the element count per row is 1,
 * while the repeat count should be large enough to hold all characters.
 * For `T != std::string`, the element count per row is the repeat count.
 *
 * @note
 * Since the values are stored sequentially even for vector columns,
 * a scalar column can be "fold" into a vector column by
 * just setting a repeat count greater than 1, and vice-versa.
 * This trick allows writing scalar columns as vector columns,
 * which is what CFitsIO recommends for performance.
 * Indeed, with CFitsIO, it is much faster to write 1 row with a repeat count of 10,000
 * than 10,000 rows with a repeat count of 1.
 * This is because binary tables are written row-wise in the Fits file.
 * @note
 * CFitsIO uses an internal buffer, which can be exploited to optimize reading and writing.
 * This is generally handled through the "iterator function" provided by CFitsIO.
 * @note
 * Fortunately, this complexity is already embedded in EleFits internals:
 * the buffer is used optimally when reading and writing several columns.
 * In general, it is nearly as fast to read and write scalar columns as vector columns with EleFits.
 * Therefore, users are encouraged to consider the repeat count as a meaningful value,
 * rather than as an optimization trick.
 * 
 * @see \ref optim
 * @see \ref data_classes
 */
template <typename T, long N = 1>
struct ColumnInfo {

  /**
   * @brief The value type.
   */
  using Value = T;

  /**
   * @brief The dimension parameter.
   */
  static constexpr long Dim = N;

  /**
   * @brief Create a column info in which only the first component of the shape is different from 1.
   * @param n The column name
   * @param u The column unit
   * @param r The repeat count
   */
  ColumnInfo(std::string n = "", std::string u = "", long r = 1);

  /**
   * @brief Create a column info with given entry shape.
   * @param n The column name
   * @param u The column unit
   * @param s The entry shape
   */
  ColumnInfo(std::string n, std::string u, Position<N> s = Position<N>::one());

  /**
   * @brief The column name.
   */
  std::string name;

  /**
   * @brief The column unit.
   */
  std::string unit = "";

  /**
   * @brief The shape of one entry.
   */
  Position<N> shape;

  /**
   * @brief Get the repeat count of the column, i.e., number of values per entry.
   */
  long repeatCount() const;

  /**
   * @brief Get the number of elements per entry.
   * @details
   * This is generally the repeat count,
   * except for string columns for which it always equals 1.
   */
  long elementCountPerEntry() const;
};

/**
 * @relates ColumnInfo
 * @brief `ColumnInfo` equality operator.
 */
template <typename T, long N>
bool operator==(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs);

/**
 * @relates ColumnInfo
 * @brief `ColumnInfo` unequality operator.
 */
template <typename T, long N>
bool operator!=(const ColumnInfo<T, N>& lhs, const ColumnInfo<T, N>& rhs);

} // namespace Fits
} // namespace Euclid

#define _ELEFITSDATA_COLUMNINFO_IMPL
#include "EleFitsData/impl/ColumnInfo.hpp"
#undef _ELEFITSDATA_COLUMNINFO_IMPL

#endif // _ELEFITSDATA_COLUMNINFO_H
