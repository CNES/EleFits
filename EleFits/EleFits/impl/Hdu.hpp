// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined(_ELEFITS_HDU_IMPL) || defined(CHECK_QUALITY)

#include "EleCfitsioWrapper/HeaderWrapper.h"
#include "EleFits/Hdu.h"
#include "EleFitsData/RecordVec.h"

namespace Fits {

/**
 * @brief Specialization of `Hdu::as()` for the header unit.
 */
template <>
const Header& Hdu::as() const;

template <typename T>
const T& Hdu::as() const
{
  return dynamic_cast<const T&>(*this);
}

} // namespace Fits

#endif
