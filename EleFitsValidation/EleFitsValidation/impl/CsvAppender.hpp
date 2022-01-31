// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/CsvAppender.h"

#if defined(_ELEFITS_VALIDATION_CSVAPPENDER_IMPL) || defined(CHECK_QUALITY)

namespace Euclid {
namespace Fits {
namespace Validation {

template <typename T>
CsvAppender& CsvAppender::operator<<(const T& value) {
  m_file << value << m_sep;
  return *this;
}

template <typename... Ts>
CsvAppender& CsvAppender::writeRow(const Ts&... values) {
  // TODO check size
  using mockUnpack = int[];
  (void)mockUnpack {(operator<<(values), 0)...};
  return operator<<(std::endl);
}

} // namespace Validation
} // namespace Fits
} // namespace Euclid

#endif
