// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFits/MefFile.h"

namespace Fits {

HduIterator<> begin(MefFile& f)
{
  return {f, 0};
}

HduIterator<> end(MefFile& f)
{
  return {f, f.hdu_count()};
}

} // namespace Fits
