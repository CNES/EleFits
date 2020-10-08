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

#ifndef _EL_FITSDATA_TESTCOLUMN_H
#define _EL_FITSDATA_TESTCOLUMN_H

#include <complex>
#include <tuple>
#include <string>

#include "EL_FitsData/Column.h"
#include "EL_FitsData/TestUtils.h"

namespace Euclid {
namespace FitsIO {
namespace Test {

/**
 * @brief Loop over supported column types.
 * @param MACRO A two-parameter macro: the C++ type and a valid variable name to represent it
 * @see Program EL_FitsIO_PrintSupportedTypes to display all supported types
 * @see EL_FITSIO_FOREACH_RECORD_TYPE
 * @see EL_FITSIO_FOREACH_RASTER_TYPE
 */
#define EL_FITSIO_FOREACH_COLUMN_TYPE(MACRO) \
  /* MACRO(bool, bool) // TODO Could be supported at some point */ \
  MACRO(char, char) \
  MACRO(std::int16_t, int16) \
  MACRO(std::int32_t, int32) \
  MACRO(std::int64_t, int64) \
  MACRO(float, float) \
  MACRO(double, double) \
  MACRO(std::complex<float>, complex_float) \
  MACRO(std::complex<double>, complex_double) \
  MACRO(std::string, string) \
  MACRO(unsigned char, uchar) \
  MACRO(std::uint16_t, uint16) \
  MACRO(std::uint32_t, uint32) \
  MACRO(std::uint64_t, uint64)

/**
 * @brief A set of random columns whihch cover the whole set of supported types.
 */
struct RandomTable {

  /**
   * @brief Generate the table.
   * @param repeatCount The repeat count of each column
   */
  RandomTable(long repeatCount = 1, long rowCount = 3);

  /**
   * @brief Generate a column.
   * @param typeName The value type name
   * @param repeatCount The repeat count
   * @details
   */
  template <typename T>
  static VecColumn<T> generateColumn(const std::string &typeName, long repeatCount = 1, long rowCount = 3);

  /**
   * @brief Get the column with given value type.
   */
  template <typename T>
  const VecColumn<T> &getColumn() const;

  /**
   * @brief Get the column with given value type.
   */
  template <typename T>
  VecColumn<T> &getColumn();

  /**
   * @brief The columns.
   */
  std::tuple<
      VecColumn<char>,
      VecColumn<std::int16_t>,
      VecColumn<std::int32_t>,
      VecColumn<std::int64_t>,
      VecColumn<float>,
      VecColumn<double>,
      VecColumn<std::complex<float>>,
      VecColumn<std::complex<double>>,
      VecColumn<std::string>,
      VecColumn<unsigned char>,
      VecColumn<std::uint16_t>,
      VecColumn<std::uint32_t>,
      VecColumn<std::uint64_t>>
      columns;

  /** @brief The number of columns. */
  static constexpr long columnCount = 13;
}; // namespace Test

/**
 * @brief A small set of columns with various types.
 */
class SmallTable {

public:
  /**
   * @brief Type of the NUM column.
   */
  using Num = int;

  /**
   * @brief Type of the RADEC column.
   */
  using Radec = std::complex<float>;

  /**
   * @brief Type of the NAME column.
   */
  using Name = std::string;

  /**
   * @brief Type of the DIST_MAG column.
   */
  using DistMag = double;

  /**
   * @brief Generate the columns.
   */
  SmallTable();

  /**
   * @brief HDU name.
   */
  std::string extname;

  /**
   * @brief Values of the NUM column.
   */
  std::vector<Num> nums;

  /**
   * @brief Values of the RADEC column.
   */
  std::vector<Radec> radecs;

  /**
   * @brief Values of the NAME column.
   */
  std::vector<Name> names;

  /**
   * @brief Values of the DIST_MAG column.
   */
  std::vector<DistMag> distsMags;

  /**
   * @brief NUM column.
   */
  VecRefColumn<Num> numCol;

  /**
   * @brief RADEC column.
   */
  VecRefColumn<Radec> radecCol;

  /**
   * @brief NAME column.
   */
  VecRefColumn<Name> nameCol;

  /**
   * @brief DIST_MAG column.
   */
  VecRefColumn<DistMag> distMagCol;
};

/**
 * @brief A random scalar Column of given type.
 */
template <typename T>
class RandomScalarColumn : public VecColumn<T> {

public:
  /**
   * @brief Generate a Column of given size.
   */
  explicit RandomScalarColumn(long size = 3, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomScalarColumn() = default;
};

/**
 * @brief A small vector column of given type.
 */
template <typename T>
class RandomVectorColumn : public VecColumn<T> {

public:
  /**
   * @brief Generate a Column.
   */
  explicit RandomVectorColumn(long repeatCount = 3, long size = 3, T min = almostMin<T>(), T max = almostMax<T>());

  /** @brief Destructor. */
  virtual ~RandomVectorColumn() = default;
};

} // namespace Test
} // namespace FitsIO
} // namespace Euclid

/// @cond INTERNAL
#define _EL_FITSDATA_TESTCOLUMN_IMPL
#include "EL_FitsData/impl/TestColumn.hpp"
#undef _EL_FITSDATA_TESTCOLUMN_IMPL
/// @endcond

#endif
