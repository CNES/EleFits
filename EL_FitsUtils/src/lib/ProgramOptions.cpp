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

#include "EL_FitsUtils/ProgramOptions.h"

#include "EL_FitsUtils/StringUtils.h"

namespace Euclid {
namespace FitsIO {

ProgramOptions::ProgramOptions(const std::string& helpMessage) :
    m_named { makeDesc(helpMessage) }, m_add { m_named.add_options() }, m_positional {} {}

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

} // namespace FitsIO
} // namespace Euclid
