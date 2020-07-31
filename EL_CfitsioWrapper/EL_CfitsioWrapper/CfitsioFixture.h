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

#ifndef _EL_CFITSIOWRAPPER_CFITSIOFIXTURE_H
#define _EL_CFITSIOWRAPPER_CFITSIOFIXTURE_H

#include <string>

#include <fitsio.h>

#include "ElementsKernel/Temporary.h"

namespace Euclid {
namespace FitsIO {

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
   * @brief The handler which generates the name and removes the file at desctruction.
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
   * @brief Pointer to the fitsfile for use with CFitsIO routines.
   */
  fitsfile* fptr;

};

}
}
}

#endif
