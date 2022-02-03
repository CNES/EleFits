// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
struct TestBintable { // FIXME add multidimensional column

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
