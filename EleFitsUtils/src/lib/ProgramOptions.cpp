// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsUtils/ProgramOptions.h"

#include "EleFitsUtils/StringUtils.h"

namespace Euclid {
namespace Fits {

ProgramOptions::ProgramOptions(const std::string& help_message) :
    m_named {make_desc(help_message)}, m_add {m_named.add_options()}, m_positional {} {}

ProgramOptions ProgramOptions::from_aux_file(const std::string& help_file) {
  return ProgramOptions(String::read_aux_file(help_file));
}

void ProgramOptions::named(const char* name, const ProgramOptions::ValueSemantics* value, const char* description) {
  m_add(name, value, description);
}

void ProgramOptions::positional(
    const char* name,
    const ProgramOptions::ValueSemantics* value,
    const char* description) {
  m_add(name, value, description);
  const int max_args = value->max_tokens();
  m_positional.add(name, max_args);
}

void ProgramOptions::flag(const char* name, const char* description) {
  named(name, boost::program_options::value<bool>()->default_value(false)->implicit_value(true), description);
}

std::pair<ProgramOptions::OptionsDescription, ProgramOptions::PositionalOptionsDescription>
ProgramOptions::as_pair() const {
  return std::make_pair(m_named, m_positional);
}

std::string ProgramOptions::make_desc(const std::string& help_message) {
  const std::string options_group = "Specific options";
  if (help_message.length() > 0) {
    return String::trim(help_message, "\n") + "\n\n" + options_group;
  }
  return options_group;
}

} // namespace Fits
} // namespace Euclid
