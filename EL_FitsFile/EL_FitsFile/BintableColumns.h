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

#ifndef _EL_FITSFILE_BINTABLECOLUMNS_H
#define _EL_FITSFILE_BINTABLECOLUMNS_H

#include "EL_FitsData/Column.h"
#include "EL_FitsData/DataUtils.h"

#include <fitsio.h>
#include <functional>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup bintable_handlers
 * @brief Column-wise reader-writer for the binary table data unit.
 * @warning NOT YET IMPLEMENTED!
 */
class BintableColumns {

private:
  friend class BintableHdu;

  /**
   * @brief Constructor.
   */
  BintableColumns(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @name Column metadata.
   */
  /// @{

  /**
   * @brief Check whether the HDU contains a given column.
   * @warning This is a read operation.
   */
  bool has(const std::string& name) const;

  /**
   * @brief Get the index of the column with given name.
   */
  long readIndex(const std::string& name) const;

  /**
   * @brief Get the name of the column with given index.
   */
  std::string readName(long index) const;

  /**
   * @brief Get the names of all the columns.
   */
  std::vector<std::string> readNames() const;

  /**
   * @brief Rename the column with given name.
   */
  void rename(const std::string& name, const std::string& newName) const;

  /**
   * @brief Rename the column with given index.
   */
  void rename(long index, const std::string& newName) const;

  /// @}
  /**
   * @name Read a single column.
   */
  /// @{

  /**
   * @brief Read the column with given name.
   */
  template <typename T>
  VecColumn<T> read(const std::string& name) const;

  /**
   * @brief Read the column with given index.
   */
  template <typename T>
  VecColumn<T> read(long index) const;

  /// @}
  /**
   * @name Read a sequence of columns.
   */
  /// @{

  /**
   * @brief Read the columns with given names and types.
   * @details
   * Example usage:
   * \code
   * auto columns = ext.readSeq(Named<int>("A"), Named<float>("B"), Named<std::string>("C"));
   * \endcode
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readSeq(const Named<Ts>&... names) const;

  /**
   * @brief Read the columns with given indices and types.
   * @details
   * Example usage:
   * \code
   * auto columns = ext.read(Indexed<int>(0), Indexed<float>(3), Indexed<std::string>(4));
   * \endcode
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readSeq(const Indexed<Ts>&... indices) const;

  /// @}
  /**
   * @name Write a single column.
   */
  /// @{

  /**
   * @brief Write a column.
   */
  template <typename T>
  void write(const Column<T>& column) const;

  /**
   * @brief Append or insert a column, which was not previously initialized.
   * @param column The column
   * @param index The column index, which may be negative (e.g. -1 to append the column at the end)
   */
  template <typename T>
  void insert(const Column<T>& column, long index = -1) const;

  /// @}
  /**
   * @name Write a sequence of columns.
   */
  /// @{

  /**
   * @brief Write several columns.
   */
  template <typename... Ts>
  void writeSeq(const Column<Ts>&... columns) const;

  /**
   * @brief Write several columns.
   */
  template <typename TSeq>
  void writeSeq(TSeq&& columns) const;

  /**
   * @brief Append several columns.
   */
  template <typename... Ts>
  void appendSeq(const Column<Ts>&... columns) const;

  /// @}

private:
  /**
   * @brief The fitsfile.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The function to declare that the header was touched.
   */
  std::function<void(void)> m_touch;

  /**
   * @brief The function to declare that the header was edited.
   */
  std::function<void(void)> m_edit;

}; // End of BintableColumns class

} // namespace FitsIO
} // namespace Euclid

#endif
