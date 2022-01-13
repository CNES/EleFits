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
namespace Fits {

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
   * @brief Read a column with given name or index.
   */
  template <typename T, long N = 1>
  VecColumn<T, N> readColumn(ColumnKey key) const;

  /**
   * @brief Write a column.
   */
  template <typename TColumn>
  void writeColumn(const TColumn& column) const;

private:
  /**
   * @brief The column-wise data unit handler.
   */
  BintableColumns m_columns;
};

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_BINTABLEHDU_IMPL
#include "EleFits/impl/BintableHdu.hpp"
#undef _ELEFITS_BINTABLEHDU_IMPL
/// @endcond

#endif
