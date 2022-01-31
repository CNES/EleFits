// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleCfitsioWrapper/CfitsioFixture.h"

#include "EleCfitsioWrapper/FileWrapper.h"

namespace Euclid {
namespace Fits {
namespace Test {

MinimalFile::MinimalFile() : tmp(), filename(tmp.path().string()), fptr(nullptr) {
  fptr = Cfitsio::FileAccess::createAndOpen(filename, Cfitsio::FileAccess::CreatePolicy::OverWrite);
}

MinimalFile::~MinimalFile() {
  Cfitsio::FileAccess::close(fptr);
}

} // namespace Test
} // namespace Fits
} // namespace Euclid
