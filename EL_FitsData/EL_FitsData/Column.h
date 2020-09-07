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
#include <tuple>
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
   * including the \c '\0' character.
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
   * For strings, CFitsIO requires nelements to be just the number of rows.
   */
  virtual long nelements() const = 0;

  /**
   * @brief Number of rows in the column.
   */
  long rows() const;

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
   * @param nelements The number of elements in the column,
   * which is the number of rows for scalar and string columns.
   * @param data Pointer to the first element of the data.
   */
  PtrColumn(ColumnInfo<T> info, long nelements, const T *data);

  /** @see Column::nelements */
  long nelements() const override;

  /** @see Column::data */
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

  /** @see Column::nelements */
  long nelements() const override;

  /** @see Column::data */
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
   * @code VecColumn column(info, std::move(vector)); @endcode
   */
  VecColumn(ColumnInfo<T> columnInfo, std::vector<T> vector);

  /** @see Column::nelements */
  long nelements() const override;

  /** @see Column::data */
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
   * @code std::vector<T> v = std::move(column.vector()); @endcode
   */
  std::vector<T> &vector();

private:
  std::vector<T> m_vec;
};

///////////////
// INTERNAL //
/////////////

/// @cond INTERNAL
namespace internal {

/**
 * @brief Implementation for Column::rows to dispatch std::string and other types.
 */
template <typename T>
long rowsImpl(long nelements, long repeat);

/**
 * std::string dispatch.
 */
template <>
long rowsImpl<std::string>(long nelements, long repeat);

/**
 * Other types dispatch.
 */
template <typename T>
long rowsImpl(long nelements, long repeat) {
  return (nelements + repeat - 1) / repeat;
}

} // namespace internal
/// @endcond

/////////////////////
// IMPLEMENTATION //
///////////////////

template <typename T>
Column<T>::Column(ColumnInfo<T> columnInfo) : info(columnInfo) {
}

template <typename T>
long Column<T>::rows() const {
  return internal::rowsImpl<T>(nelements(), info.repeat);
}

template <typename T>
PtrColumn<T>::PtrColumn(ColumnInfo<T> columnInfo, long nelements, const T *data) :
    Column<T>(columnInfo),
    m_nelements(nelements),
    m_data(data) {
}

template <typename T>
long PtrColumn<T>::nelements() const {
  return m_nelements;
}

template <typename T>
const T *PtrColumn<T>::data() const {
  return m_data;
}

template <typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> columnInfo, const std::vector<T> &vectorRef) :
    Column<T>(columnInfo),
    m_ref(vectorRef) {
}

template <typename T>
long VecRefColumn<T>::nelements() const {
  return m_ref.size();
}

template <typename T>
const T *VecRefColumn<T>::data() const {
  return m_ref.data();
}

template <typename T>
const std::vector<T> &VecRefColumn<T>::vector() const {
  return m_ref;
}

template <typename T>
VecColumn<T>::VecColumn() : Column<T>({ "", "", 1 }), m_vec() {
}

template <typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> columnInfo, std::vector<T> vector) : Column<T>(columnInfo), m_vec(vector) {
}

template <typename T>
long VecColumn<T>::nelements() const {
  return m_vec.size();
}

template <typename T>
const T *VecColumn<T>::data() const {
  return m_vec.data();
}

template <typename T>
T *VecColumn<T>::data() {
  return m_vec.data();
}

template <typename T>
const std::vector<T> &VecColumn<T>::vector() const {
  return m_vec;
}

template <typename T>
std::vector<T> &VecColumn<T>::vector() {
  return m_vec;
}

} // namespace FitsIO
} // namespace Euclid

#endif
