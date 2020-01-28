/**
 * @file EL_FitsData/Column.h
 * @date 10/21/19
 * @author user
 *
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
 * @brief Column info, i.e. { name, repeat, unit }
 */
template<typename T>
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
   * @brief Repeat count of the column, i.e. number of values per cell.
   * @warning CFitsIO uses long instead of size_t
   */
  long repeat;

};


/**
 * @brief Bintable column data and metadata.
 */
template<typename T>
class Column {

public:

  virtual ~Column() = default;
  Column(ColumnInfo<T> info);

  /**
   * @brief Number of elements in the column, i.e. number of rows * repeat count.
   * @warning For strings, CFitsIO requires nelements to be just the number of rows.
   */
  virtual std::size_t nelements() const = 0; //TODO long?

  std::size_t rows() const;

  /**
   * @brief Access the data.
   */
  virtual const T* data() const = 0;

  /**
   * @brief Column metadata.
   */
  ColumnInfo<T> info;

};


/**
 * @brief Column which references some external pointer data.
 * @details Use it for temporary columns.
 */
template<typename T>
class PtrColumn : public Column<T> {

public:

  virtual ~PtrColumn() = default;
  PtrColumn(const PtrColumn&) = default;
  PtrColumn(PtrColumn&&) = default;
  PtrColumn& operator=(const PtrColumn&) = default;
  PtrColumn& operator=(PtrColumn&&) = default;

  PtrColumn(ColumnInfo<T> info, std::size_t nelements, const T* data);

  virtual std::size_t nelements() const;

  virtual const T* data() const;

private:

  std::size_t m_nelements;
  const T* m_data;

};


/**
 * @brief Column which references some external vector data.
 * @details Use it for temporary columns.
 */
template<typename T>
class VecRefColumn : public Column<T> {

public:

  virtual ~VecRefColumn() = default;
  VecRefColumn(const VecRefColumn&) = default;
  VecRefColumn(VecRefColumn&&) = default;
  VecRefColumn& operator=(const VecRefColumn&) = default;
  VecRefColumn& operator=(VecRefColumn&&) = default;

  VecRefColumn(ColumnInfo<T> info, const std::vector<T>& vector);

  virtual std::size_t nelements() const;

  virtual const T* data() const;

  const std::vector<T>& vector() const;

private:

  const std::vector<T>& m_vec_ref;

};


/**
 * @brief Column which stores internally the data.
 * @details Use it (via move semantics) if you don't need your data after the write operation.
 */
template<typename T>
class VecColumn : public Column<T> {

public:

  virtual ~VecColumn() = default;
  VecColumn(const VecColumn&) = default;
  VecColumn(VecColumn&&) = default;
  VecColumn& operator=(const VecColumn&) = default;
  VecColumn& operator=(VecColumn&&) = default;

  VecColumn();

  VecColumn(ColumnInfo<T> info, std::vector<T> vector);

  virtual std::size_t nelements() const;

  virtual const T* data() const;

  T* data();

  const std::vector<T>& vector() const;

  /**
   * @brief Non-const reference to the data, useful to take ownership through move semantics.
   * @code
   * std::vector<T> v = std::move(column.vector());
   * @endcode
   */
  std::vector<T>& vector();

private:

  std::vector<T> m_vec;

};


///////////////
// INTERNAL //
/////////////

namespace internal {

template<typename T>
std::size_t _rows(std::size_t nelements, long repeat);

template<>
std::size_t _rows<std::string>(std::size_t nelements, long repeat);

template<typename T>
std::size_t _rows(std::size_t nelements, long repeat) {
  return nelements / repeat; //TODO dangerous although nelements should be a multiple of repeat
}

}

/////////////////////
// IMPLEMENTATION //
///////////////////


template<typename T>
Column<T>::Column(ColumnInfo<T> info) :
    info(info) {
}

template<typename T>
std::size_t Column<T>::rows() const {
  return internal::_rows<T>(nelements(), info.repeat);
}

template<typename T>
PtrColumn<T>::PtrColumn(ColumnInfo<T> info, std::size_t nelements, const T* data) :
    Column<T>(info),
    m_nelements(nelements),
    m_data(data) {
}

template<typename T>
std::size_t PtrColumn<T>::nelements() const {
  return m_nelements;
}

template<typename T>
const T* PtrColumn<T>::data() const {
  return m_data;
}


template<typename T>
VecRefColumn<T>::VecRefColumn(ColumnInfo<T> info, const std::vector<T>& vector) :
    Column<T>(info),
    m_vec_ref(vector) {
}

template<typename T>
std::size_t VecRefColumn<T>::nelements() const {
  return m_vec_ref.size();
}

template<typename T>
const T* VecRefColumn<T>::data() const {
  return m_vec_ref.data();
}

template<typename T>
const std::vector<T>& VecRefColumn<T>::vector() const {
  return m_vec_ref;
}


template<typename T>
VecColumn<T>::VecColumn() :
    Column<T>({ "", "", 1 }),
    m_vec() {
}

template<typename T>
VecColumn<T>::VecColumn(ColumnInfo<T> info, std::vector<T> vector) :
    Column<T>(info),
    m_vec(vector) {
}

template<typename T>
std::size_t VecColumn<T>::nelements() const {
  return m_vec.size();
}

template<typename T>
const T* VecColumn<T>::data() const {
  return m_vec.data();
}

template<typename T>
T* VecColumn<T>::data() {
  return m_vec.data();
}

template<typename T>
const std::vector<T>& VecColumn<T>::vector() const {
  return m_vec;
}

template<typename T>
std::vector<T>& VecColumn<T>::vector() {
  return m_vec;
}


}
}

#endif
