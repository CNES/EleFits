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

#include <string>
#include <vector>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Column metadata, i.e. { name, unit, repeat }
 * @see \ref data-classes
 */
template <typename T>
struct ColumnInfo {

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
  long repeat;
};

/**
 * @brief Bintable column data and metadata.
 * @details
 * This is an interface to be implemented with a concrete data container (e.g. std::vector).
 * Some implementations are provided with the library,
 * but others could be useful to interface with client code
 * (e.g. with other external libraries with custom containers).
 * @see \ref data-classes
 */
template <typename T>
class Column {

public:
  /**
   * @brief Destructor.
   */
  virtual ~Column() = default;

  /**
   * @brief Create a column with given metadata.
   */
  explicit Column(ColumnInfo<T> columnInfo);

  /**
   * @brief Number of elements in the column, i.e. number of rows * repeat count.
   * @warning
   * For strings, CFitsIO requires elementCount to be just the number of rows.
   */
  virtual long elementCount() const = 0;

  /**
   * @brief Number of rows in the column.
   */
  long rowCount() const;

  /**
   * @brief Const pointer to the first data element.
   */
  virtual const T *data() const = 0;

  /**
   * @brief Column metadata.
   */
  ColumnInfo<T> info;
};

/**
 * @brief Column which references some external pointer data.
 * @details
 * Use it for temporary columns.
 * @see \ref data-classes
 */
template <typename T>
class PtrColumn : public Column<T> {

public:
  /** @brief Destructor. */
  virtual ~PtrColumn() = default;
  /** @brief Copy constructor. */
  PtrColumn(const PtrColumn &) = default;
  /** @brief Move constructor. */
  PtrColumn(PtrColumn &&) = default;
  /** @brief Copy assignment. */
  PtrColumn &operator=(const PtrColumn &) = default;
  /** @brief Move assignment. */
  PtrColumn &operator=(PtrColumn &&) = default;

  /**
   * @brief Create a new column with given metadata and data.
   * @param info The column metadata.
   * @param elementCount The number of elements in the column,
   * which is the number of rows for scalar and string columns.
   * @param data Pointer to the first element of the data.
   */
  PtrColumn(ColumnInfo<T> info, long elementCount, const T *data);

  /** @copydoc Column::elementCount */
  long elementCount() const override;

  /** @copydoc Column::data */
  const T *data() const override;

private:
  long m_nelements;
  const T *m_data;
};

/**
 * @brief Column which references some external vector data.
 * @details Use it for temporary columns.
 * @see \ref data-classes
 */
template <typename T>
class VecRefColumn : public Column<T> {

public:
  /** @brief Destructor. */
  virtual ~VecRefColumn() = default;
  /** @brief Copy constructor. */
  VecRefColumn(const VecRefColumn &) = default;
  /** @brief Move constructor. */
  VecRefColumn(VecRefColumn &&) = default;
  /** @brief Copy assignment. */
  VecRefColumn &operator=(const VecRefColumn &) = default;
  /** @brief Move assignment. */
  VecRefColumn &operator=(VecRefColumn &&) = default;

  /**
   * @brief Create a VecRefColumn with given metadata and reference to data.
   */
  VecRefColumn(ColumnInfo<T> columnInfo, const std::vector<T> &vectorRef);

  /** @copydoc Column::elementCount */
  long elementCount() const override;

  /** @copydoc Column::data */
  const T *data() const override;

  /**
   * @brief Const reference to the vector data.
   */
  const std::vector<T> &vector() const;

private:
  const std::vector<T> &m_ref;
};

/**
 * @brief Column which stores internally the data.
 * @details Use it (via move semantics) if you don't need your data after the write operation.
 * @see \ref data-classes
 */
template <typename T>
class VecColumn : public Column<T> {

public:
  /** @brief Destructor. */
  virtual ~VecColumn() = default;
  /** @brief Copy constructor. */
  VecColumn(const VecColumn &) = default;
  /** @brief Move constructor. */
  VecColumn(VecColumn &&) = default;
  /** @brief Copy assignment. */
  VecColumn &operator=(const VecColumn &) = default;
  /** @brief Move assignment. */
  VecColumn &operator=(VecColumn &&) = default;

  /**
   * @brief Create an empty VecColumn.
   */
  VecColumn();

  /**
   * @brief Crate a VecColumn with given data and metadata.
   * @details
   * To transfer ownership of the data instead of copying it, use move semantics:
   * \code
   * VecColumn column(info, std::move(vector));
   * \endcode
   */
  VecColumn(ColumnInfo<T> columnInfo, std::vector<T> vector);

  /** @copydoc Column::elementCount */
  long elementCount() const override;

  /** @copydoc Column::data */
  const T *data() const override;

  /**
   * @brief Non-const pointer to the first data element.
   */
  T *data();

  /**
   * @brief Const reference to the vector data.
   */
  const std::vector<T> &vector() const;

  /**
   * @brief Non-const reference to the data, useful to take ownership through move semantics.
   * \code
   * std::vector<T> v = std::move(column.vector());
   * \endcode
   */
  std::vector<T> &vector();

private:
  std::vector<T> m_vec;
};

} // namespace FitsIO
} // namespace Euclid

#include "impl/Column.hpp"

#endif
