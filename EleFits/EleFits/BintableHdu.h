// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_BINTABLEHDU_H
#define _ELEFITS_BINTABLEHDU_H

#include "EleCfitsioWrapper/BintableWrapper.h"
#include "EleFits/BintableColumns.h"
#include "EleFits/Hdu.h"

#include <string>

namespace Fits {

/**
 * @ingroup bintable_handlers
 * @brief Binary table HDU reader-writer.
 */
class BintableHdu : public Hdu {
public:

  /// @group_construction

  /// @cond INTERNAL

  /**
   * @see Hdu
   */
  BintableHdu(Token, fitsfile*& fptr, Linx::Index index, HduCategory status = HduCategory::Untouched);

  /**
   * @see Hdu
   */
  BintableHdu();

  /// @endcond

  /**
   * @brief Destructor.
   */
  LINX_VIRTUAL_DTOR(BintableHdu)

  /// @group_properties

  /**
   * @copydoc Hdu::category
   */
  HduCategory category() const override;

  /// @group_elements

  /**
   * @brief Access the data unit column-wise.
   * @see BintableColumns
   */
  const BintableColumns& columns() const;

  /// @group_operations

  /**
   * @brief Read the number of columns.
   */
  Linx::Index read_column_count() const;

  /**
   * @brief Read the number of rows.
   */
  Linx::Index read_row_count() const;

  /**
   * @brief Read a column with given name or index.
   */
  template <typename T, Linx::Index N = 1>
  VecColumn<T, N> read_column(ColumnKey key) const;

  /**
   * @brief Write a column.
   */
  template <typename TColumn>
  void write_column(const TColumn& column) const;

  /// @}

private:

  /**
   * @brief The column-wise data unit handler.
   */
  BintableColumns m_columns;
};

} // namespace Fits

/// @cond INTERNAL
#define _ELEFITS_BINTABLEHDU_IMPL
#include "EleFits/impl/BintableHdu.hpp"
#undef _ELEFITS_BINTABLEHDU_IMPL
/// @endcond

#endif
