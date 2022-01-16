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

#ifndef _ELEFITSDATA_COLUMN_H
#define _ELEFITSDATA_COLUMN_H

#include "EleFitsData/ColumnInfo.h"
#include "EleFitsData/DataContainer.h"
#include "EleFitsData/DataUtils.h"
#include "EleFitsData/Raster.h" // PtrRaster for Entry

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/// @cond INTERNAL
// Issue with forward declarations: https://github.com/doxygen/doxygen/issues/8177

// Forward declaration for PtrColumn and VecColumn
template <typename T, long N, typename TContainer>
class ColumnContainer;

/// @endcond

/**
 * @ingroup bintable_data_classes
 * @brief Column which references some external pointer data.
 * @details
 * Use it for temporary columns.
 * @see \ref data_classes
 */
template <typename T, long N = 1>
using PtrColumn = ColumnContainer<T, N, T*>;

/**
 * @ingroup bintable_data_classes
 * @brief Column which stores internally the data.
 * @details
 * Use it (via move semantics) if you don't need your data after the write operation.
 * @see \ref data_classes
 */
template <typename T, long N = 1>
using VecColumn = ColumnContainer<T, N, std::vector<T>>;

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
template <typename T, long N, typename TContainer>
class ColumnContainer : public DataContainer<T, TContainer, ColumnContainer<T, N, TContainer>> {

  /**
   * @brief Shortcut for DataContainer.
   */
  using Base = DataContainer<T, TContainer, ColumnContainer<T, N, TContainer>>;

public:
  /**
   * @brief The info type.
   */
  using Info = ColumnInfo<std::decay_t<T>, N>;

  /**
   * @brief The element value type.
   */
  using Value = T; // FIXME Info::Value, i.e. decay

  /**
   * @brief The entry value type.
   */
  using Entry = PtrRaster<T, N>;

  /**
   * @brief The dimension parameter.
   */
  static constexpr long Dim = Info::Dim;

  /**
   * @name Constructors
   */
  /// @{

  ELEFITS_VIRTUAL_DTOR(ColumnContainer)
  ELEFITS_COPYABLE(ColumnContainer)
  ELEFITS_MOVABLE(ColumnContainer)

  /**
   * @brief Default constructor.
   */
  ColumnContainer();

  /**
   * @brief Create an empty column with given metadata.
   * @param info The column metadata
   * @param rowCount The row count
   */
  explicit ColumnContainer(Info info, long rowCount = 0);

  /**
   * @brief Create a column with given metadata and data.
   * @param info The column metadata
   * @param rowCount The row count
   * @param data The raw data
   * @warning
   * `PtrColumn` constructor used to get the **element** count as input instead of the **row count**,
   * which makes a difference for vector columns.
   */
  ColumnContainer(Info info, long rowCount, T* data);

  /**
   * @brief Create a column with given metadata and data.
   * @param info The column metadata
   * @param args Arguments to be forwarded to the underlying container
   */
  template <typename... Ts>
  ColumnContainer(Info info, Ts&&... args);

  /// @}
  /**
   * @name Properties
   */
  /// @{

  /**
   * @brief Get the column metadata.
   */
  const Info& info() const;

  /**
   * @brief Change the column name.
   */
  void rename(const std::string& name);

  /**
   * @brief Change the column repeat count (fold/unfold).
   * @details
   * The repeat count must be a divisor of the columns size, except for string columns.
   */
  void reshape(long repeatCount = 1);

  /**
   * @brief Number of elements in the column, i.e. repeat count * number of rows.
   * @warning
   * For string columns, CFitsIO requires elementCount to be just the number of rows,
   * although they are vector columns.
   * @deprecated Use standard `size()` instead.
   */
  long elementCount() const;

  /**
   * @brief Number of rows in the column.
   */
  long rowCount() const;

  /// @}
  /**
   * @name Element access
   */
  /// @{

  using Base::operator[];

  /**
   * @details
   * Three methods are available to access elements:
   * 
   * - Method data() returns a pointer to the first element.
   * - Methods operator()() provide access to the value at given row and repeat indices;
   * - Methods at() additionally perform bound checking and allows for backward (negative) indexing;
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

  /// @}
  /**
   * @name Views
   */
  /// @{

  /**
   * @brief Get a view on contiguous rows.
   */
  const PtrColumn<const T> slice(const Segment& rows) const;

  /**
   * @copydoc slice()
   */
  PtrColumn<T> slice(const Segment& rows);

  /// @}

private:
  /**
   * @brief Column metadata.
   */
  Info m_info;
};

/**
 * @relates Column
 * @brief Shortcut to create a column from a column info and data without specifying the template parameters.
 * @tparam T The value type, should not be specified (automatically deduced)
 * @param data The column values, which can be either a pointer (or C array) or a vector
 * @param info The column info
 * @details
 * Example usage:
 * \code
 * auto column = makeColumn(std::move(vector), std::move(info)); // Copy-less
 * \endcode
 */
template <typename T, typename TInfo>
PtrColumn<T, TInfo::Dim> makeColumn(TInfo&& info, long rowCount, T* data) {
  return {std::forward<TInfo>(info), rowCount, data};
}

/**
 * @relates Column
 * @copydoc makeColumn
 */
template <typename T, typename TInfo>
VecColumn<T, TInfo::Dim> makeColumn(TInfo info, std::vector<T> data) {
  return {std::forward<TInfo>(info), std::move(data)};
}

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_COLUMN_IMPL
#include "EleFitsData/impl/Column.hpp"
#undef _ELEFITSDATA_COLUMN_IMPL
/// @endcond

#endif
