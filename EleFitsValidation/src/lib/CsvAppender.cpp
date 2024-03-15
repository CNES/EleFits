// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsValidation/CsvAppender.h"

namespace Fits {
namespace Validation {

CsvAppender::CsvAppender(const std::string& filename, const std::vector<std::string>& header, const std::string& sep) :
    m_file(filename, std::ios::out | std::ios::app), m_sep(sep)
{
  if (header.empty()) {
    return;
  }
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  const bool append = in.tellg(); // position is 0 if file was just created
  if (append) {
    // TODO check header consistency
  } else {
    for (const auto& h : header) {
      (*this) << h;
    }
    (*this) << std::endl;
  }
}

CsvAppender& CsvAppender::operator<<(std::ostream& (*pf)(std::ostream&))
{
  m_file << pf;
  return *this;
}

} // namespace Validation
} // namespace Fits
