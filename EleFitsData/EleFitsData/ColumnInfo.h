/**
 * @copyright (C) 2012-2022 CNES (for the Euclid Science Ground Segment)
 *
 * This file is part of EleFits.
 * 
 * EleFits is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * EleFits is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with EleFits.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _ELEFITSDATA_COLUMNINFO_H
#define _ELEFITSDATA_COLUMNINFO_H

#include "EleFitsData/Position.h"

#include <complex>
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
 * @brief Column informations, i.e. name, unit, entry shape and value type.
 * @tparam T The value type
 * @tparam N The dimension (number of axes per entry)
 * @details
 * Binary tables can be seen as a sequence of columns made of consecutive entries.
 * Entry values are not necessarily simple types:
 * they can be either string, scalar, vector or multidimensional.
 * Yet, all the entries of one column have the same properties.
 * The column informations consist in:
 * - The value type as template parameter `T`;
 * - The dimension (number of axes) as template parameter `N`;
 * - The column `name`;
 * - The column `unit`;
 * - The entry `shape` (see below).
 * 
 * The number of values per entry, named repeat count, is the shape size.
 * Here is a table which summarizes the properties of each column type
 * -- and more details follow.
 * 
 * <table class="fieldtable">
 * <tr><th>Category<th>`T`<th>`N`<th>`repeatCount()`<th>`elementCount()`<th>`shape`
 * <tr><td>%String<td>`std::string`<td>1<td>> max number of characters<td>1<td>`{repeatCount()}`
 * <tr><td>Scalar<td>Not `std::string`<td>1<td>1<td>1<td>`{1}`
 * <tr><td>Vector<td>Not `std::string`<td>1<td>> 1<td> = repeat count<td>`{repeatCount()}`
 * <tr><td>Multidimensional<td>Not `std::string`<td>-1 or > 1<td>= shape size<td>= shape size<td>Unconstrained
 * </table>
 * 
 * For string columns, the element count differs from the repeat count,
 * in that the element count is the number of `std::string` objects stored in the column data container,
 * while the repeat count is the number of characters allocated to each string in the Fits file.
 * This results in `elementCount()` = 1 and `repeatCount()` > 1
 * (multidimensional string columns are not supported).
 * 
 * @par_example
 * Here is an example of a 4-row table with each column category:
 * 
 * <table class="fieldtable">
 * <tr><th>Row<th>%String<th>Scalar<th>Vector<th>Multidim
 * <tr><td>0<td>`"ZERO"`<td>0<td>00 01 02<td>000 001 002<br>010 011 012
 * <tr><td>1<td>`"ONE"`<td>1<td>10 11 12<td>100 101 102<br>110 111 112
 * <tr><td>2<td>`"TWO"`<td>2<td>20 21 22<td>200 201 202<br>210 211 212
 * <tr><td>3<td>`"THREE"`<td>3<td>30 31 32<td>300 301 302<br>310 311 312
 * </table>
 * 
 * For performance, the values are stored sequentially in a 1D array as follows:
 * \code
 * ColumnInfo<std::string> stringInfo("String", "", 6);
 * std::string stringData[] = {"ZERO", "ONE", "TWO", "THREE"};
 * 
 * ColumnInfo<int> scalarInfo("Scalar");
 * int scalarData[] = {0, 1, 2, 3};
 * 
 * ColumnInfo<int> vectorInfo("Vector", "", 3);
 * int vectorData[] = {
 *     00, 01, 02,
 *     10, 11, 12,
 *     20, 21, 22,
 *     30, 31, 32};
 * 
 * ColumnInfo<int, 2> multidimInfo("Multidim", "", {3, 2});
 * int multidimData[] = {
 *     000, 001, 002, 010, 011, 012,
 *     100, 101, 102, 110, 111, 112,
 *     200, 201, 202, 210, 211, 212,
 *     300, 301, 302, 310, 311, 312};
 * \endcode
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
 * @see `Column`
 * @see \ref optim
 */
template <typename T, long N = 1>
struct ColumnInfo {

  /**
   * @brief The value decay type.
   */
  using Value = std::decay_t<T>;

  /**
   * @brief The dimension parameter.
   */
  static constexpr long Dim = N;

  /**
   * @brief Create a column info with given entry repeat count.
   * @param n The column name
   * @param u The column unit
   * @param r The repeat count
   * @details
   * The entry shape is deduced from the repeat count
   * (first axis' lenght is the repeat count, others are set to 1).
   */
  ColumnInfo(std::string n = "", std::string u = "", long r = 1) : name(n), unit(u), shape(Position<N>::one()) {
    shape[0] = r;
  }

  /**
   * @brief Create a column info with given entry shape.
   * @param n The column name
   * @param u The column unit
   * @param s The entry shape
   * @details
   * The repeat count is deduced from the shape.
   */
  ColumnInfo(std::string n, std::string u, Position<N> s) : name(n), unit(u), shape(std::move(s)) {}

  /**
   * @brief The column name.
   */
  std::string name;

  /**
   * @brief The column unit.
   */
  std::string unit;

  /**
   * @brief The shape of one entry.
   */
  Position<N> shape;

  /**
   * @brief Get the repeat count.
   */
  long repeatCount() const;

  /**
   * @brief Get the number of elements per entry.
   */
  long elementCount() const;
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

/**
 * @relates ColumnInfo
 * @brief Shortcut to create column informations without specifying the dimension template parameters.
 * @tparam T The value type, must be specified
 * @tparam Longs The axes lengths, should not be specified (automatically deduced)
 * @param name The column name
 * @param unit The column unit, or `""` to omit it
 * @param shape The shape as a comma-separated list of `long`s
 * can be left empty for scalar columns
 * 
 * @par_example
 * \code
 * auto stringInfo = makeColumnInfo<std::string>("String", "", 6);
 * auto scalarInfo = makeColumnInfo<int>("Scalar");
 * auto vectorInfo = makeColumnInfo<int>("Vector", "", 3);
 * auto multidimInfo = makeColumnInfo<int>("Multidim", "", 3, 2);
 * \endcode
 */
template <typename T, typename... Longs>
ColumnInfo<T, sizeof...(Longs)> makeColumnInfo(const std::string& name, const std::string& unit, Longs... shape) {
  return {name, unit, Position<sizeof...(Longs)> {shape...}};
}

/**
 * @relates ColumnInfo
 * @brief Scalar column specialization.
 */
template <typename T>
ColumnInfo<T> makeColumnInfo(const std::string& name, const std::string& unit = "") {
  return {name, unit};
}

} // namespace Fits
} // namespace Euclid

#define _ELEFITSDATA_COLUMNINFO_IMPL
#include "EleFitsData/impl/ColumnInfo.hpp"
#undef _ELEFITSDATA_COLUMNINFO_IMPL

#endif // _ELEFITSDATA_COLUMNINFO_H
