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

#ifndef _ELEFITS_BINTABLEHDU_H
#define _ELEFITS_BINTABLEHDU_H

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleFits/BintableColumns.h"
#include "EleFits/Hdu.h"

#include <string>

namespace Euclid {
namespace FitsIO {

/**
 * @ingroup bintable_handlers
 * @brief Binary table HDU reader-writer.
 */
class BintableHdu : public Hdu {

public:
  /// @cond INTERNAL

  /**
   * @see Hdu
   */
  BintableHdu(Token, fitsfile*& fptr, long index, HduCategory status = HduCategory::Untouched);

  /**
   * @see Hdu
   */
  BintableHdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  virtual ~BintableHdu() = default;

  /**
   * @brief Access the data unit column-wise.
   * @see BintableColumns
   */
  const BintableColumns& columns() const;

  /**
   * @brief Read the number of columns.
   */
  long readColumnCount() const;

  /**
   * @brief Read the number of rows.
   */
  long readRowCount() const;

  /**
   * @copydoc Hdu::readCategory
   */
  HduCategory readCategory() const override;

  /**
   * @brief Check whether the HDU contains a given column.
   * @warning This is a read operation.
   */
  bool hasColumn(const std::string& name) const;

  /**
   * @brief Check whether the HDU contains a given set of columns.
   * @warning This is a read operation.
   * @see hasColumn
   */
  std::vector<bool> hasColumns(const std::vector<std::string>& names) const;

  /**
   * @brief Get the index of the column with given name.
   */
  long readColumnIndex(const std::string& name) const;

  /**
   * @brief Get the name of the column with given index.
   */
  std::string readColumnName(long index) const;

  /**
   * @brief Get the names of all the columns.
   */
  std::vector<std::string> readColumnNames() const;

  /**
   * @brief Rename the column with given name.
   */
  void renameColumn(const std::string& name, const std::string& newName) const;

  /**
   * @brief Rename the column with given index.
   */
  void renameColumn(long index, const std::string& newName) const;

  /**
   * @brief Read a column with given index.
   */
  template <typename T>
  VecColumn<T> readColumn(long index) const;

  /**
   * @brief Read a column with given name.
   */
  template <typename T>
  VecColumn<T> readColumn(const std::string& name) const;

  /**
   * @brief Read several columns with given indices.
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const std::vector<long>& indices) const;

  /**
   * @brief Same as readColumns(const std::vector<long> &) with modified signature.
   * @details
   * This allows writing the type of each column next to its index, e.g.:
   * \code
   * auto columns = ext.readColumns(Indexed<int>(0), Indexed<float>(3), Indexed<std::string>(4));
   * \endcode
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const Indexed<Ts>&... indices) const;

  /**
   * @brief Read several columns with given names.
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const std::vector<std::string>& names) const;

  /**
   * @brief Same as readColumns(const std::vector<std::string> &) with modified signature.
   * @details
   * This allows writing the type of each column next to its name, e.g.:
   * \code
   * auto columns = ext.readColumns(Named<int>("A"), Named<float>("B"), Named<std::string>("C"));
   * \endcode
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const Named<Ts>&... names) const;

  /**
   * @brief Write a column.
   */
  template <typename T>
  void writeColumn(const Column<T>& column) const;

  /**
   * @brief Write several columns.
   * @warning
   * All columns should have the same number of rows.
   */
  template <typename... Ts>
  void writeColumns(const Column<Ts>&... columns) const;

  /**
   * @brief Append a column.
   * @warning
   * The column should have the same number of rows as the existing columns.
   */
  template <typename T>
  void appendColumn(const Column<T>& column) const;

  /**
   * @brief Append several columns.
   * @warning
   * All new columns should have the same number of rows as the existing columns.
   */
  template <typename... Ts>
  void appendColumns(const Column<Ts>&... columns) const;

private:
  /**
   * @brief The column-wise data unit handler.
   */
  BintableColumns m_columns;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_BINTABLEHDU_IMPL
#include "EleFits/impl/BintableHdu.hpp"
#undef _ELEFITS_BINTABLEHDU_IMPL
/// @endcond

#endif
