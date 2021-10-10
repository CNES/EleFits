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

#ifndef _EL_FITSDATA_COLUMN_H
#define _EL_FITSDATA_COLUMN_H

#include "EL_FitsData/DataUtils.h"

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup bintable_data_classes
 * @brief Loop over supported column types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it.
 * @see Program EL_FitsIO_PrintSupportedTypes to display all supported types.
 * @see EL_FITSIO_FOREACH_RECORD_TYPE
 * @see EL_FITSIO_FOREACH_RASTER_TYPE
 */
#define EL_FITSIO_FOREACH_COLUMN_TYPE(MACRO) \
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
 * @brief Column metadata, i.e. `{ name, unit, repeatCount }`
 * and the value type as the template parameter.
 * @details
 * Binary table columns are either scalar (`repeatCount` = 1) or vector (`repeatCount` > 1).
 * In the case of vector columns, each cell of the column contains `repeatCount` values.
 * Here is an example of a 4-row table with a scalar column and a vector column with a repeat count of 3:
 * <table>
 * <tr><th>Row<th>repeatCount = 1<th>repeatCount = 3
 * <tr style="text-align:center"><td>0<td>00<td>00, 01, 02
 * <tr style="text-align:center"><td>1<td>10<td>10, 11, 12
 * <tr style="text-align:center"><td>2<td>20<td>20, 21, 22
 * <tr style="text-align:center"><td>3<td>30<td>30, 31, 32
 * </table>
 * For performance, the values are stored sequentially in a 1D array as follows:
 * \code
 * int repeat1[] = { 00, 10, 20, 30 };
 * int repeat3[] = { 00, 01, 02, 10, 11, 12, 20, 21, 22, 30, 31, 32 };
 * \endcode
 *
 * The only exception to this is string columns, which are vector columns
 * -- they should have a repeat count greater than the maximum number of characters in a cell --
 * but each cell contains only one string:
 * <table>
 * <tr><th>Row<th>repeatCount = 6
 * <tr><td>0<td>`"ZERO"`
 * <tr><td>1<td>`"ONE"`
 * <tr><td>2<td>`"TWO"`
 * <tr><td>3<td>`"THREE"`
 * </table>
 * The data array is a simple array of `std::string`s:
 * \code
 * std::string data[] = { "ZERO", "ONE", "TWO", "THREE", };
 * \endcode
 * but the repeat count should be at least 6 (beware of the null terminating character).
 *
 * @note
 * Since the values are stored sequentially even for vector columns,
 * a scalar column can be "fold" into a vector column by
 * just setting a repeat count greater than 1, and vice-versa.
 * This trick allows writing scalar columns as vector columns,
 * which is what CFitsIO recommends for performance.
 * Indeed, with CFitsIO, it is much faster to write 1 row with a repeat count of 10.000
 * than 10.000 rows with a repeat count of 1.
 * This is because binary tables are written row-wise in the Fits file.
 * CFitsIO uses an internal buffer, which can be exploited to optimize reading and writing.
 * This is generally handled through the "iterator function" provided by CFitsIO.
 * @note
 * Fortunately, this complexity is already embedded in EL_FitsIO internals:
 * the buffer is used optimally when reading and writing several columns.
 * In general, it is nearly as fast to read and write scalar columns as vector columns with EL_FitsIO.
 * Therefore, users are encouraged to consider the repeat count as a meaningful value,
 * rather than as an optimization trick.
 *
 * @see \ref optim
 * @see \ref data_classes
 */
template <typename T>
struct ColumnInfo {

  /**
   * @brief The value type.
   */
  using Value = T;

  /**
   * @brief Constructor.
   */
  ColumnInfo(const std::string& columnName, const std::string& columnUnit = "", long columnRepeatCount = 1);

  /**
   * @brief Column name.
   */
  std::string name;

  /**
   * @brief Column unit.
   */
  std::string unit;

  /**
   * @brief Repeat count of the column, i.e., number of values per cell.
   * @details
   * Scalar columns have a repeat count of 1.
   * @warning
   * String columns are considered vector columns.
   * Their repeat count must be greater or equal to the longest string of the column
   * including the `\0` character.
   */
  long repeatCount;
};

// Forward declaration for Column::slice()
template <typename T>
class PtrColumn;

/**
 * @ingroup bintable_data_classes
 * @brief Binary table column data and metadata.
 * @details
 * This is an interface to be implemented with a concrete data container (e.g. `std::vector`).
 * Some implementations are provided with the library,
 * but others could be useful to interface with client code
 * (e.g. with other external libraries with custom containers).
 * @see \ref data_classes
 */
template <typename T>
class Column {

public:
  /**
   * @brief The element value type.
   */
  using Value = T;

  /**
   * @brief Create a column with given metadata.
   */
  explicit Column(ColumnInfo<T> columnInfo);

  /**
   * @brief Destructor.
   */
  virtual ~Column() = default;

  /**
   * @name Get properties
   */
  /// @{

  /**
   * @brief Number of elements in the column, i.e. repeat count * number of rows.
   * @warning
   * For string columns, CFitsIO requires elementCount to be just the number of rows,
   * although they are vector columns.
   */
  virtual long elementCount() const = 0;

  /**
   * @brief Number of rows in the column.
   */
  long rowCount() const;

  /// @}
  /**
   * @name Access elements
   */
  /// @{

  /**
   * @details
   * Three methods are available to access elements:
   * 
   * - Methods operator()() provide access to the value at given row and repeat indices;
   * - Methods at() additionally perform bound checking and allows for backward (negative) indexing;
   * - Method data() returns a pointer to the first element.
   * 
   * @param row The row index
   * @param repeat The repeat index
   */

  /**
   * @brief Access the value at given row and repeat indices.
   */
  const T& operator()(long row, long repeat = 0) const;

  /**
   * @copydoc operator()()
   */
  T& operator()(long row, long repeat = 0);

  /**
   * @copydoc operator()()
   */
  const T& at(long row, long repeat = 0) const;

  /**
   * @copydoc at()
   */
  T& at(long row, long repeat = 0);

  /**
   * @copydoc operator()()
   */
  virtual const T* data() const = 0;

  /**
   * @brief Pointer to the first data element.
   * @details
   * If the Column is read-only, returns `nullptr`.
   */
  virtual T* data(); // TODO throw?

  /// @}
  /**
   * @name Slicing
   */
  /// @{

  /**
   * @brief Get a view on contiguous rows.
   */
  const PtrColumn<T> slice(const Segment& rows) const;

  /**
   * @copydoc slice()
   */
  PtrColumn<T> slice(const Segment& rows);

  /// @}

public:
  /**
   * @brief Column metadata.
   */
  ColumnInfo<T> info;
};

/**
 * @ingroup bintable_data_classes
 * @brief Column which references some external pointer data.
 * @details
 * Use it for temporary columns.
 * @see \ref data_classes
 */
template <typename T>
class PtrColumn : public Column<T> {

public:
  /**
   * @brief Destructor. 
   */
  virtual ~PtrColumn() = default;

  /**
   * @brief Copy constructor.
   */
  PtrColumn(const PtrColumn&) = default;

  /**
   * @brief Move constructor.
   */
  PtrColumn(PtrColumn&&) = default;

  /**
   * @brief Copy assignment.
   */
  PtrColumn& operator=(const PtrColumn&) = default;

  /**
   * @brief Move assignment.
   */
  PtrColumn& operator=(PtrColumn&&) = default;

  /**
   * @brief Create a new column with given metadata and data.
   * @param info The column metadata.
   * @param elementCount The number of elements in the column,
   * which is the number of rows for scalar and string columns.
   * @param data Pointer to the first element of the data.
   */
  PtrColumn(ColumnInfo<T> info, long elementCount, const T* data);

  /**
   * @brief Create a new column with given metadata and data.
   * @param info The column metadata.
   * @param elementCount The number of elements in the column,
   * which is the number of rows for scalar and string columns.
   * @param data Pointer to the first element of the data.
   */
  PtrColumn(ColumnInfo<T> info, long elementCount, T* data);

  /**
   * @copydoc Column::elementCount()
   */
  long elementCount() const override;

  /**
   * @copydoc Column::data() const
   */
  const T* data() const override;

  /**
   * @copydoc Column::data()
   */
  T* data() override;

private:
  long m_nelements;
  const T* m_cData;
  T* m_data;
};

/**
 * @ingroup bintable_data_classes
 * @brief Column which references some external vector data.
 * @details
 * Use it for temporary columns.
 * @see \ref data_classes
 */
template <typename T>
class VecRefColumn : public Column<T> {

public:
  /**
   * @brief Destructor.
   */
  virtual ~VecRefColumn() = default;

  /**
   * @brief Copy constructor.
   */
  VecRefColumn(const VecRefColumn&) = default;

  /**
   * @brief Move constructor.
   */
  VecRefColumn(VecRefColumn&&) = default;

  /**
   * @brief Copy assignment.
   */
  VecRefColumn& operator=(const VecRefColumn&) = default;

  /**
   * @brief Move assignment.
   */
  VecRefColumn& operator=(VecRefColumn&&) = default;

  /**
   * @brief Create a VecRefColumn with given metadata and reference to data.
   */
  VecRefColumn(ColumnInfo<T> info, const std::vector<T>& vecRef);

  /**
   * @brief Create a VecRefColumn with given metadata and reference to data.
   */
  VecRefColumn(ColumnInfo<T> info, std::vector<T>& vecRef);

  /**
   * @copydoc Column::elementCount()
   */
  long elementCount() const override;

  /**
   * @copydoc Column::data() const
   */
  const T* data() const override;

  /**
   * @copydoc Column::data()
   */
  T* data() override;

  /**
   * @brief Const reference to the vector data.
   */
  const std::vector<T>& vector() const;

private:
  const std::vector<T>* m_cVecPtr;
  std::vector<T>* m_vecPtr;
};

/**
 * @ingroup bintable_data_classes
 * @brief Column which stores internally the data.
 * @details
 * Use it (via move semantics) if you don't need your data after the write operation.
 * @see \ref data_classes
 */
template <typename T>
class VecColumn : public Column<T> {

public:
  /**
   * @brief Destructor.
   */
  virtual ~VecColumn() = default;

  /**
   * @brief Copy constructor.
   */
  VecColumn(const VecColumn&) = default;

  /**
   * @brief Move constructor.
   */
  VecColumn(VecColumn&&) = default;

  /**
   * @brief Copy assignment.
   */
  VecColumn& operator=(const VecColumn&) = default;

  /**
   * @brief Move assignment.
   */
  VecColumn& operator=(VecColumn&&) = default;

  /**
   * @brief Create an empty VecColumn.
   */
  VecColumn();

  /**
   * @brief Crate a VecColumn with given data and metadata.
   * @details
   * To transfer ownership of the data instead of copying it, use move semantics:
   * \code
   * VecColumn column(info, std::move(vec));
   * \endcode
   */
  VecColumn(ColumnInfo<T> info, std::vector<T> vec);

  /**
   * @brief Create a VecColumn with given metadata.
   */
  VecColumn(ColumnInfo<T> info, long rowCount);

  /**
   * @copydoc Column::elementCount()
   */
  long elementCount() const override;

  /**
   * @copydoc Column::data() const
   */
  const T* data() const override;

  /**
   * @brief Non-const pointer to the first data element.
   */
  T* data() override;

  /**
   * @brief Const reference to the vector data.
   */
  const std::vector<T>& vector() const;

  /**
   * @brief Non-const reference to the data, useful to take ownership through move semantics.
   * \code
   * std::vector<T> v = std::move(column.vector());
   * \endcode
   */
  std::vector<T>& vector();

private:
  std::vector<T> m_vec;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSDATA_COLUMN_IMPL
#include "EL_FitsData/impl/Column.hpp"
#undef _EL_FITSDATA_COLUMN_IMPL
/// @endcond

#endif
