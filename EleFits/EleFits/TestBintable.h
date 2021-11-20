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

#ifndef _ELEFITS_TESTBINTABLE_H
#define _ELEFITS_TESTBINTABLE_H

#include "EleFits/FitsFileFixture.h" // FIXME rename as TestFitsFile.h
#include "EleFitsData/TestColumn.h"

namespace Euclid {
namespace Fits {
namespace Test {

/**
 * @brief Temporary MEF file with one binary table extension.
 * @details
 * The table is made of a scalar and a vector column of same value type.
 */
template <typename T>
struct TestBintable {

  /**
   * @brief Constructor.
   */
  TestBintable(long rows = 10) :
      scalarColumn(rows), vectorColumn(3, rows), firstColumn(scalarColumn), lastColumn(vectorColumn), file(),
      hdu(file.initBintableExt("BINTABLE", firstColumn.info(), lastColumn.info())), columns(hdu.columns()) {
    assert(scalarColumn.info().name != vectorColumn.info().name);
  }

  /**
   * @brief The scalar column.
   */
  Test::RandomScalarColumn<T> scalarColumn;

  /**
   * @brief The vector column.
   */
  Test::RandomVectorColumn<T> vectorColumn;

  /**
   * @brief A reference to the first column.
   */
  VecColumn<T>& firstColumn;

  /**
   * @brief A reference to the last column.
   */
  VecColumn<T>& lastColumn;

  /**
   * @brief The MEF file.
   */
  Test::TemporaryMefFile file;

  /**
   * @brief The HDU.
   */
  const BintableHdu& hdu;

  /**
   * @brief The data unit.
   */
  const BintableColumns& columns;
};

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
