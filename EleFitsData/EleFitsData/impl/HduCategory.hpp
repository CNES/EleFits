// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITSDATA_HDUCATEGORY_IMPL) || defined(CHECK_QUALITY)

#include "EleFitsData/HduCategory.h"

namespace Fits {

class Hdu;
class Header;
class ImageHdu;
class ImageRaster;
class BintableHdu;
class BintableColumns;

template <>
HduCategory HduCategory::forClass<Hdu>();

template <>
HduCategory HduCategory::forClass<Header>();

template <>
HduCategory HduCategory::forClass<ImageHdu>();

template <>
HduCategory HduCategory::forClass<ImageRaster>();

template <>
HduCategory HduCategory::forClass<BintableHdu>();

template <>
HduCategory HduCategory::forClass<BintableColumns>();

} // namespace Fits

#endif
