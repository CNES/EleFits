// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELECFITSIOWRAPPER_CFITSIOFIXTURE_H
#define _ELECFITSIOWRAPPER_CFITSIOFIXTURE_H

#include "ElementsKernel/Temporary.h"

#include <fitsio.h>
#include <string>

namespace Euclid {
namespace Fits {

/**
 * @brief Test-related classes and functions.
 */
namespace Test {

/**
 * @brief A minimal and temporary FITS file removed by destructor.
 */
class MinimalFile {

protected:
  /**
   * @brief The handler which generates the name and removes the file at destruction.
   */
  Elements::TempFile tmp;

public:
  /**
   * @brief Create a file with generated filename.
   */
  MinimalFile();

  /**
   * @brief Remove the file.
   */
  ~MinimalFile();

  /**
   * @brief Name of the file.
   */
  std::string filename;

  /**
   * @brief Pointer to the fitsfile for use with CFITSIO routines.
   */
  fitsfile* fptr;
};

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
