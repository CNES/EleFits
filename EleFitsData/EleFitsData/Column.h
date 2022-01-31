// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_COLUMN_H
#define _ELEFITSDATA_COLUMN_H

#include "EleFitsData/ColumnInfo.h"
#include "EleFitsData/DataContainer.h"
#include "EleFitsData/Raster.h" // PtrRaster for Entry
#include "EleFitsData/Segment.h"

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/// @cond
// Issue with forward declarations: https://github.com/doxygen/doxygen/issues/8177

// Forward declaration for PtrColumn and VecColumn
template <typename T, long N, typename TContainer>
class Column;

/// @endcond

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which points to some external data (`TContainer` = `T*`).
 */
template <typename T, long N = 1>
using PtrColumn = Column<T, N, T*>;

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which owns a data vector (`TContainer` = `std::vector<T>`).
 */
template <typename T, long N = 1>
using VecColumn = Column<T, N, std::vector<T>>;

/**
 * @ingroup bintable_data_classes
 * @tparam T The value type, possibly const-qualified for read-only columns
 * @tparam N The entry dimension (number of axes) or -1 for runtime dimension
 * @tparam TContainer The data container, which must meet `SizedData` requirements
 * @brief Binary table column data and metadata.
 * @details
 * A column is a contiguous container for the entry data of a binary table column.
 * As explained in the `ColumnInfo` documentation (make sure to have read it before going further),
 * entries can be made of several values.
 * Template parameter `N` is bound to the entry category:
 * - `N` = 1 for scalar, string and vector columns;
 * - `N` > 1 for multidimensional columns with fixed dimension;
 * - `N` = -1 for multidimensional columns with runtime dimension.
 * 
 * @tspecialization{PtrColumn}
 * @tspecialization{VecColumn}
 * 
 * @satisfies{ContiguousContainer}
 * @satisfies{VectorArithmetic}
 * 
 * @see `ColumnInfo` for details on the entry properties.
 * @see `makeColumn()` for creation shortcuts.
 */
template <typename T, long N, typename TContainer>
class Column : public DataContainer<T, TContainer, Column<T, N, TContainer>> {

  /**
   * @brief Shortcut for DataContainer.
   */
  using Base = DataContainer<T, TContainer, Column<T, N, TContainer>>;

public:
  /**
   * @brief The element value type.
   */
  using Value = std::decay_t<T>;

  /**
   * @brief The dimension parameter.
   */
  static constexpr long Dim = N;

  /**
   * @brief The info type.
   */
  using Info = ColumnInfo<Value, N>;

  /// @group_construction

  ELEFITS_VIRTUAL_DTOR(Column)
  ELEFITS_COPYABLE(Column)
  ELEFITS_MOVABLE(Column)

  /**
   * @brief Default constructor.
   */
  Column();

  /**
   * @brief Create an empty column with given metadata.
   * @param info The column metadata
   * @param rowCount The row count
   */
  explicit Column(Info info, long rowCount = 0);

  /**
   * @brief Create a column with given metadata and data.
   * @param info The column metadata
   * @param rowCount The row count
   * @param data The raw data
   * @warning
   * `PtrColumn` constructor used to get the **element** count as input instead of the **row count**,
   * which makes a difference for vector columns.
   */
  Column(Info info, long rowCount, T* data);

  /**
   * @brief Create a column with given metadata and data.
   * @param info The column metadata
   * @param args Arguments to be forwarded to the underlying container
   */
  template <typename... Ts>
  Column(Info info, Ts&&... args);

  /// @group_properties

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
   * The repeat count must be a divisor of the column size, except for string columns.
   * The resulting entry shape will be flat, with the first component = `repeatCount`
   * and the other components = 1.
   * @see reshape(Position<N>)
   */
  void reshape(long repeatCount = 1);

  /**
   * @brief Change the entry shape.
   * @details
   * The shape size must be a divisor of the column size.
   * @see reshape(long)
   */
  void reshape(Position<N> shape);

  /**
   * @brief Number of elements in the column, i.e. repeat count times number of rows.
   * @warning
   * For string columns, the element count is just the number of rows,
   * although they are vector columns.
   * @deprecated Use standard `size()` instead.
   */
  long elementCount() const;

  /**
   * @brief Number of rows in the column.
   */
  long rowCount() const;

  /// @group_elements

  using Base::operator[];

  /**
   * @brief Access the value at given row and repeat indices.
   * @param row The row index
   * @param repeat The repeat index
   * @details
   * Several methods are available to access elements:
   * 
   * - `data()` returns a pointer to the first element;
   * - `operator()()` gives access to the element at given row and repeat indices;
   * - `at()` additionally perform bound checking and allows for backward (negative) indexing.
   */
  const T& operator()(long row, long repeat = 0) const;

  /**
   * @copybrief operator()(long,long)const
   */
  T& operator()(long row, long repeat = 0);

  /**
   * @copybrief operator()(long,long)const
   */
  const T& at(long row, long repeat = 0) const;

  /**
   * @copybrief at()
   */
  T& at(long row, long repeat = 0);

  /// @group_views

  /**
   * @brief Access the entry at given row as a raster.
   */
  const PtrRaster<const T, N> entry(long row) const;

  /**
   * @copybrief entry(long)const
   */
  PtrRaster<T, N> entry(long row);

  /**
   * @brief Get a view on contiguous rows.
   */
  const PtrColumn<const T, N> slice(const Segment& rows) const;

  /**
   * @copybrief slice()
   */
  PtrColumn<T, N> slice(const Segment& rows);

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
 * @param info The column info
 * @param data The column values, which can be either a pointer (or C array) or a vector
 * @details
 * Example usage:
 * \code
 * auto column = makeColumn(std::move(info), std::move(vector)); // Copy-less
 * \endcode
 */
template <typename TInfo, typename TContainer>
Column<typename TContainer::value_type, std::decay_t<TInfo>::Dim, TContainer>
makeColumn(TInfo info, TContainer&& data) {
  return {std::forward<TInfo>(info), std::forward<TContainer>(data)};
}

/**
 * @relates Column
 * @brief Pointer specialization.
 */
template <typename T, typename TInfo>
PtrColumn<T, std::decay_t<TInfo>::Dim> makeColumn(TInfo&& info, long rowCount, T* data) {
  return {std::forward<TInfo>(info), rowCount, data};
}

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_COLUMN_IMPL
#include "EleFitsData/impl/Column.hpp"
#undef _ELEFITSDATA_COLUMN_IMPL
/// @endcond

#endif
