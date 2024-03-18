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

namespace Fits {

/// @cond
// Issue with forward declarations: https://github.com/doxygen/doxygen/issues/8177

// Forward declaration for PtrColumn and VecColumn
template <typename T, Linx::Index N, typename THolder>
class Column;

/// @endcond

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which points to some external data (`PtrHolder` is a mere wrapper of `T*`).
 */
template <typename T, Linx::Index N = 1>
using PtrColumn = Column<T, N, Linx::PtrHolder<T>>;

/**
 * @ingroup bintable_data_classes
 * @brief `Column` which owns a vector (`THolder` = `std::vector<T>`).
 * 
 * The data holder `StdHolder` supports move semantics,
 * such that `VecColumn`s can be converted to/from `std::vector`s at negligible cost.
 * 
 * @warning Booleans are not supported since `std::vector<bool>` is _not_ a contiguous container.
 */
template <typename T, Linx::Index N = 1>
using VecColumn = Column<T, N, Linx::StdHolder<std::vector<T>>>;

/**
 * @ingroup bintable_data_classes
 * @tparam T The value type, possibly const-qualified for read-only columns
 * @tparam N The field dimension (number of axes) or -1 for runtime dimension
 * @tparam THolder The data container, which must meet `SizedData` requirements
 * @brief Binary table column data and metadata.
 * 
 * A column is a contiguous container for binary table columns.
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
  using Container = Linx::DataContainer<T, THolder, Linx::EuclidArithmetic, Column<T, N, THolder>>;

public:

  /**
   * @brief The element value type.
   */
  using Value = std::decay_t<T>;

  /**
   * @brief The dimension parameter.
   */
  static constexpr Linx::Index Dimension = N;

  /**
   * @brief The info type.
   */
  using Info = ColumnInfo<Value, N>;

  /// @group_construction

  LINX_VIRTUAL_DTOR(Column)
  LINX_DEFAULT_COPYABLE(Column)
  LINX_DEFAULT_MOVABLE(Column)

  /**
   * @brief Default constructor, for compatibility.
   */
  Column();

  /**
   * @brief Create a column with given metadata and optional data.
   * @param info The column metadata
   * @param row_count The row count
   * @param args Optional data holder arguments
   */
  template <typename... TArgs>
  explicit Column(Info info, Linx::Index row_count = 0, TArgs&&... args) :
      Container(info.element_count() * row_count, LINX_FORWARD(args)...), m_info(std::move(info))
  {}

  /**
   * @brief Create a column from iterators.
   * @param info The column metadata
   * @param list The row-major ordered list of elements
   * @param args Optional data holder arguments
   */
  template <typename TIt, typename... TArgs>
  explicit Column(Info info, TIt begin, TIt end, TArgs&&... args) :
      Container(begin, end, LINX_FORWARD(args)...), m_info(LINX_MOVE(info))
  {}

  /**
   * @brief Create a column from given initialization list.
   * @param info The column metadata
   * @param list The row-major ordered list of elements
   * @param args Optional data holder arguments
   */
  template <typename TRange, typename... TArgs>
  explicit Column(Info info, std::initializer_list<T> list, TArgs&&... args) :
      Container(list.begin(), list.end(), LINX_FORWARD(args)...), m_info(LINX_MOVE(info))
  {}

  /**
   * @brief Create a column from given range.
   * @param info The column metadata
   * @param list The row-major ordered container of elements
   * @param args Optional data holder arguments
   */
  template <typename TRange, std::enable_if_t<Linx::IsRange<TRange>::value>* = nullptr, typename... TArgs>
  explicit Column(Info info, TRange&& range, TArgs&&... args) :
      Container(range.size(), LINX_FORWARD(range), LINX_FORWARD(args)...), m_info(LINX_MOVE(info))
  {}

  /**
   * @brief Create a column from given read-only range.
   * @param info The column metadata
   * @param list The row-major ordered container of elements
   * @param args Optional data holder arguments
   */
  template <typename TRange, std::enable_if_t<Linx::IsRange<TRange>::value>* = nullptr, typename... TArgs>
  explicit Column(Info info, const TRange& range, TArgs&&... args) :
      Container(LINX_FORWARD(range).begin(), LINX_FORWARD(range).end(), LINX_FORWARD(args)...), m_info(LINX_MOVE(info))
  {}

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
   * @brief Get the number of rows in the column.
   */
  Linx::Index row_count() const;

  /// @group_elements

  using Container::operator[];

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
   * @brief Access the value at given row and repeat indices.
   * 
   * As opposed to `operator()`, this method checks bounds and supports backward indexing.
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

private:

  /**
   * @brief Column metadata.
   */
  Info m_info;
};

/// @cond
template <typename T>
struct IsStdHolderCompatible : std::false_type {}; // FIXME move to Linx

template <typename T, typename TAlloc>
struct IsStdHolderCompatible<std::vector<T, TAlloc>> : std::true_type {};

template <typename T, std::size_t N>
struct IsStdHolderCompatible<std::array<T, N>> : std::true_type {};

template <typename T>
struct IsStdHolderCompatible<std::valarray<T>> : std::true_type {};

template <typename T>
constexpr bool is_std_holder_compatible()
{
  return IsStdHolderCompatible<std::decay_t<T>>::value;
}

template <typename T>
struct ColumnDimensionImpl {
  static constexpr Linx::Index Value = 1;
};

template <typename T, Linx::Index N>
struct ColumnDimensionImpl<ColumnInfo<T, N>> {
  static constexpr Linx::Index Value = N;
};

template <typename T>
constexpr Linx::Index column_dimension()
{
  return ColumnDimensionImpl<std::decay_t<T>>::Value;
}
/// @endcond

/**
 * @relates Column
 * @brief Shortcut to create a column from a column info and data without specifying the template parameters.
 * @tparam T The value type, should not be specified (automatically deduced)
 * @param info The column name or full info
 * @param range A range of column values
 * @param row_count The number of rows
 * @param data A pointer to the column values
 * @details
 * Example usage:
 * \code
 * auto column = make_column(std::move(info), std::move(vector)); // Copy-less initialization of owning column
 * auto ptr_column = make_column(std::move(info), vector.size(), vector.data()); // Initialization of non-owning column
 * \endcode
 */
template <typename TInfo, typename TRange>
auto make_column(TInfo&& info, TRange&& range)
{
  using T = std::remove_reference_t<decltype(*range.begin())>;
  using Value = std::decay_t<T>;
  static constexpr auto N = column_dimension<TInfo>();
  if constexpr (is_std_holder_compatible<TRange>()) {
    return Column<T, N, Linx::StdHolder<TRange>>(ColumnInfo<Value, N>(LINX_FORWARD(info)), LINX_FORWARD(range));
  } else {
    return Column<Value, N, Linx::DefaultHolder<Value>>(
        ColumnInfo<Value, N>(LINX_FORWARD(info)),
        range.begin(),
        range.end());
  }
}

/**
 * @relates Column
 * @brief Pointer specialization.
 */
template <typename T, typename TInfo>
auto make_column(TInfo&& info, Linx::Index row_count, T* data)
{
  static constexpr auto N = column_dimension<TInfo>();
  return PtrColumn<T, N>(ColumnInfo<std::decay_t<T>, N>(LINX_FORWARD(info)), row_count, data);
}

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITSDATA_COLUMN_IMPL
#include "EleFitsData/impl/Column.hpp"
#undef _ELEFITSDATA_COLUMN_IMPL
/// @endcond

#endif
