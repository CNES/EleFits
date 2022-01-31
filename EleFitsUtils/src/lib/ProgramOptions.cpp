// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EleFitsUtils/ProgramOptions.h"

#include "EleFitsUtils/StringUtils.h"

namespace Euclid {
namespace Fits {

ProgramOptions::ProgramOptions(const std::string& helpMessage) :
    m_named {makeDesc(helpMessage)}, m_add {m_named.add_options()}, m_positional {} {}

ProgramOptions ProgramOptions::fromAuxFile(const std::string& helpFile) {
  return ProgramOptions(String::readAuxFile(helpFile));
}

void ProgramOptions::named(const char* name, const ProgramOptions::ValueSemantics* value, const char* description) {
  m_add(name, value, description);
}

void ProgramOptions::positional(
    const char* name,
    const ProgramOptions::ValueSemantics* value,
    const char* description) {
  m_add(name, value, description);
  const int maxArgs = value->max_tokens();
  m_positional.add(name, maxArgs);
}

void ProgramOptions::flag(const char* name, const char* description) {
  named(name, boost::program_options::value<bool>()->default_value(false)->implicit_value(true), description);
}

std::pair<ProgramOptions::OptionsDescription, ProgramOptions::PositionalOptionsDescription>
ProgramOptions::asPair() const {
  return std::make_pair(m_named, m_positional);
}

std::string ProgramOptions::makeDesc(const std::string& helpMessage) {
  const std::string optionsGroup = "Specific options";
  if (helpMessage.length() > 0) {
    return String::trim(helpMessage, "\n") + "\n\n" + optionsGroup;
  }
  return optionsGroup;
}

} // namespace Fits
} // namespace Euclid
