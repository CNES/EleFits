// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSDATA_COLUMN_H
#define _ELEFITSDATA_COLUMN_H

#include "EleFitsData/ColumnInfo.h"
#include "EleFitsData/Segment.h"
#include "Linx/Data/Raster.h" // PtrRaster for multidimensional columns

#include <complex>
#include <cstdint>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/// @cond
// Issue with forward declarations: https://github.com/doxygen/doxygen/issues/8177

// Forward declaration for PtrColumn and VecColumn
template <typename T, Linx::Index N, typename THolder>
class Column;

/// @endcond

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which points to some external data (`THolder` = `T*`).
 */
template <typename T, Linx::Index N = 1>
using PtrColumn = Column<T, N, Linx::PtrHolder<T>>;

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which owns a data vector (`THolder` = `std::vector<T>`).
 */
template <typename T, Linx::Index N = 1>
using VecColumn = Column<T, N, Linx::StdHolder<std::vector<T>>>;

/**
 * @ingroup bintable_data_classes
 * @tparam T The value type, possibly const-qualified for read-only columns
 * @tparam N The field dimension (number of axes) or -1 for runtime dimension
 * @tparam THolder The data container, which must meet `SizedData` requirements
 * @brief Binary table column data and metadata.
 * @details
 * A column is a contiguous container for the field data of a binary table column.
 * As explained in the `ColumnInfo` documentation (make sure to have read it before going further),
 * fields can be made of several values.
 * Template parameter `N` is bound to the field category:
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
 * @see `ColumnInfo` for details on the field properties.
 * @see `make_column()` for creation shortcuts.
 */
template <typename T, Linx::Index N, typename THolder = Linx::DefaultHolder<T>>
class Column : public Linx::DataContainer<T, THolder, Linx::EuclidArithmetic, Column<T, N, THolder>> {
  /**
   * @brief Shortcut for DataContainer.
   */
  using Base = Linx::DataContainer<T, THolder, Linx::EuclidArithmetic, Column<T, N, THolder>>;

public:

  /**
   * @brief The element value type.
   */
  using Value = std::decay_t<T>;

  /**
   * @brief The dimension parameter.
   */
  static constexpr Linx::Index Dim = N;

  /**
   * @brief The info type.
   */
  using Info = ColumnInfo<Value, N>;

  /// @group_construction

  LINX_VIRTUAL_DTOR(Column)
  LINX_DEFAULT_COPYABLE(Column)
  LINX_DEFAULT_MOVABLE(Column)

  /**
   * @brief Default constructor.
   */
  Column();

  /**
   * @brief Create an empty column with given metadata.
   * @param info The column metadata
   * @param row_count The row count
   */
  explicit Column(Info info, Linx::Index row_count = 0);

  /// @cond
  /** @brief int overload, to be handled differently! */
  explicit Column(Info info, int row_count) : Column(std::move(info), Linx::Index(row_count)) {}
  /// @endcond

  /**
   * @brief Create a column with given metadata and data.
   * @param info The column metadata
   * @param row_count The row count
   * @param data The raw data
   * @warning
   * `PtrColumn` constructor used to get the **element** count as input instead of the **row count**,
   * which makes a difference for vector columns.
   */
  explicit Column(Info info, Linx::Index row_count, T* data);

  /// @cond
  /** @brief int overload, to be handled differently! */
  explicit Column(Info info, int row_count, T* data) : Column(std::move(info), Linx::Index(row_count), data) {}
  /// @endcond

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
   * The resulting field shape will be flat, with the first component = `repeat_count`
   * and the other components = 1.
   * @see reshape(Linx::Position<N>)
   */
  void reshape(Linx::Index repeat_count = 1);

  /**
   * @brief Change the field shape.
   * @details
   * The shape size must be a divisor of the column size.
   * @see reshape(Linx::Index)
   */
  void reshape(Linx::Position<N> shape);

  /**
   * @brief Number of rows in the column.
   */
  Linx::Index row_count() const;

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
  const T& operator()(Linx::Index row, Linx::Index repeat = 0) const;

  /**
   * @copybrief operator()(Linx::Index,Linx::Index)const
   */
  T& operator()(Linx::Index row, Linx::Index repeat = 0);

  /**
   * @copybrief operator()(Linx::Index,Linx::Index)const
   */
  const T& at(Linx::Index row, Linx::Index repeat = 0) const;

  /**
   * @copybrief at()
   */
  T& at(Linx::Index row, Linx::Index repeat = 0);

  /// @group_views

  /**
   * @brief Access the field at given row index as a raster.
   */
  const Linx::PtrRaster<const T, N> field(Linx::Index row) const;

  /**
   * @copybrief field(Linx::Index)const
   */
  Linx::PtrRaster<T, N> field(Linx::Index row);

  /**
   * @brief Get a view on contiguous rows.
   */
  const PtrColumn<const T, N> slice(const Segment& rows) const;

  /**
   * @copybrief slice()
   */
  PtrColumn<T, N> slice(const Segment& rows);

  /// @}

  /**
   * @deprecated
   */
  Linx::Index rowCount() const
  {
    return row_count();
  }

  /**
   * @deprecated Name was wrong wrt. the FITS standard
   */
  [[deprecated("Use field(Linx::Index)")]] const Linx::PtrRaster<const T, N> entry(Linx::Index row) const
  {
    return field(row);
  }

  /**
   * @deprecated Name was wrong wrt. the FITS standard
   */
  [[deprecated("Use field(Linx::Index)")]] Linx::PtrRaster<T, N> entry(Linx::Index row)
  {
    return field(row);
  }

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
 * auto column = make_column(std::move(info), std::move(vector)); // Copy-less
 * \endcode
 */
template <typename TInfo, typename TContainer>
Column<typename TContainer::value_type, std::decay_t<TInfo>::Dim, Linx::StdHolder<TContainer>>
make_column(TInfo info, TContainer&& data)
{
  return Column<typename TContainer::value_type, std::decay_t<TInfo>::Dim, Linx::StdHolder<TContainer>> {
      std::forward<TInfo>(info),
      std::forward<TContainer>(data)};
}

/**
 * @relates Column
 * @brief Pointer specialization.
 */
template <typename T, typename TInfo>
PtrColumn<T, std::decay_t<TInfo>::Dim> make_column(TInfo&& info, Linx::Index row_count, T* data)
{
  return PtrColumn<T, std::decay_t<TInfo>::Dim> {std::forward<TInfo>(info), row_count, data};
}

/**
 * @deprecated
 */
template <typename... TArgs>
[[deprecated("Use make_column")]] auto makeColumn(TArgs&&... args)
{
  return make_column(std::forward<TArgs>(args)...);
}

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITSDATA_COLUMN_IMPL
#include "EleFitsData/impl/Column.hpp"
#undef _ELEFITSDATA_COLUMN_IMPL
/// @endcond

#endif
