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

#ifndef _EL_FITSFILE_BINTABLEHDU_H
#define _EL_FITSFILE_BINTABLEHDU_H

#include <string>

#include "EL_CfitsioWrapper/BintableWrapper.h"
#include "EL_FitsFile/RecordHdu.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Binary table HDU reader-writer.
 */
class BintableHdu : public RecordHdu {

public:
  /**
   * @brief Constructor.
   * @warning
   * You should probably not instantiate BintableHdus yourself,
   * but use the dedicated MefFile creation method
   * MefFile::initBintableExt or MefFile::assignBintableExt.
   * @todo
   * The constructor should be protected, with MefFile a friend of the class.
   */
  BintableHdu(fitsfile *&fptr, long index);

  /**
   * @brief Destructor.
   */
  virtual ~BintableHdu() = default;

  /**
   * @brief Read the number of columns.
   */
  long readColumnCount() const;

  /**
   * @brief Read the number of rows.
   */
  long readRowCount() const;

  /**
   * @brief Check whether the HDU contains a given column.
   */
  bool hasColumn(const std::string &name) const;

  /**
   * @brief Check whether the HDU contains a given set of columns.
   * @see hasColumn
   */
  std::vector<bool> hasColumns(const std::vector<std::string> &names) const;

  /**
   * @brief Read a column with given index.
   */
  template <typename T>
  VecColumn<T> readColumn(long index) const;

  /**
   * @brief Read a column with given name.
   */
  template <typename T>
  VecColumn<T> readColumn(const std::string &name) const;

  /**
   * @brief Read several columns with given indices.
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const std::vector<long> &indices) const;

  /**
   * @brief Read several columns with given names.
   */
  template <typename... Ts>
  std::tuple<VecColumn<Ts>...> readColumns(const std::vector<std::string> &names) const;

  /**
   * @brief Write a column.
   */
  template <typename T>
  void writeColumn(const Column<T> &column) const;

  /**
   * @brief Write several columns.
   * @warning
   * All columns should have the same number of rows.
   */
  template <typename... Ts>
  void writeColumns(const Column<Ts> &... columns) const;

  /**
   * @brief Append a column.
   * @warning
   * The column should have the same number of rows as the existing columns.
   */
  template <typename T>
  void appendColumn(const Column<T> &column) const;

  /**
   * @brief Append several columns.
   * @warning
   * All new columns should have the same number of rows as the existing columns.
   */
  template <typename... Ts>
  void appendColumns(const Column<Ts> &... columns) const;
};

} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSFILE_BINTABLEHDU_IMPL
#include "EL_FitsFile/impl/BintableHdu.hpp"
#undef _EL_FITSFILE_BINTABLEHDU_IMPL
/// @endcond

#endif
