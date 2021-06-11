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

namespace Euclid {
namespace FitsIO {

ProgramOptions::ProgramOptions(const std::string& helpMessage) :
    m_namedDesc { helpMessage + "\n\nSpecific options" }, m_add { m_namedDesc.add_options() }, m_positionalDesc {} {}

ProgramOptions ProgramOptions::fromAuxdir(const std::string& helpFile) {
  std::ifstream ifs(Elements::getAuxiliaryPath(helpFile).string());
  std::string helpMessage((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  return ProgramOptions(helpMessage);
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
  m_positionalDesc.add(name, maxArgs);
}

std::pair<ProgramOptions::OptionsDescription, ProgramOptions::PositionalOptionsDescription>
ProgramOptions::asPair() const {
  return std::make_pair(m_namedDesc, m_positionalDesc);
}
} // namespace FitsIO
} // namespace Euclid