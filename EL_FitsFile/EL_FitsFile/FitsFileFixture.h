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

#ifndef _EL_FITSFILE_FITSFILEFIXTURE_H
#define _EL_FITSFILE_FITSFILEFIXTURE_H

#include "ElementsKernel/Temporary.h"

#include "EL_FitsFile/MefFile.h"
#include "EL_FitsFile/SifFile.h"

namespace Euclid {
namespace FitsIO {

/**
 * @brief Valid name for temporary files.
 */
std::string temporaryFilename() {
  return Elements::TempPath("%%%%%%.fits").path().string();
}

/**
 * @brief Temporary SifFile with random name.
 */
class TemporarySifFile : public SifFile {
public:
  /** @brief Constructor */
  TemporarySifFile() :
      SifFile(temporaryFilename(), SifFile::Permission::Temporary) {}
};

/**
 * @brief SifFile with random name and create-only policy.
 */
class NewSifFile : public SifFile {
public:
  /** @brief Constructor */
  NewSifFile() :
      SifFile(temporaryFilename(), SifFile::Permission::Create) {}
};

/**
 * @brief Temporary MefFile with random name.
 */
class TemporaryMefFile : public MefFile {
public:
  /** @brief Constructor */
  TemporaryMefFile() :
      MefFile(temporaryFilename(), SifFile::Permission::Temporary) {}
};

/**
 * @brief MefFile with random name and create-only policy.
 */
class NewMefFile : public MefFile {
public:
  /** @brief Constructor */
  NewMefFile() :
      MefFile(temporaryFilename(), SifFile::Permission::Create) {}
};

}
}

#endif
