// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_FITSFILEFIXTURE_H
#define _ELEFITS_FITSFILEFIXTURE_H

#include "EleFits/MefFile.h"
#include "EleFits/SifFile.h"
#include "ElementsKernel/Temporary.h"

namespace Euclid {
namespace Fits {
namespace Test {

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
  TemporarySifFile() : SifFile(temporaryFilename(), FileMode::Temporary) {}
};

/**
 * @brief SifFile with random name and create-only policy.
 */
class NewSifFile : public SifFile {
public:
  /** @brief Constructor */
  NewSifFile() : SifFile(temporaryFilename(), FileMode::Create) {}
};

/**
 * @brief Temporary MefFile with random name.
 */
class TemporaryMefFile : public MefFile {
public:
  /** @brief Constructor */
  TemporaryMefFile() : MefFile(temporaryFilename(), FileMode::Temporary) {}
};

/**
 * @brief MefFile with random name and create-only policy.
 */
class NewMefFile : public MefFile {
public:
  /** @brief Constructor */
  NewMefFile() : MefFile(temporaryFilename(), FileMode::Create) {}
};

} // namespace Test
} // namespace Fits
} // namespace Euclid

#endif
