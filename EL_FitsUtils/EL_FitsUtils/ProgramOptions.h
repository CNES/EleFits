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

#ifndef _EL_FITSIO_UTILS_PROGRAM_H
#define _EL_FITSIO_UTILS_PROGRAM_H

#include "ElementsKernel/Auxiliary.h"

#include <boost/program_options.hpp>

namespace Euclid {
namespace FitsIO {

/**
 * @brief Helper class to declare several named options and zero or one positional option.
 */
class ProgramOptions {

public:
  using OptionsDescription = boost::program_options::options_description;
  using PositionalOptionsDescription = boost::program_options::positional_options_description;

  /**
   * @brief Destructor.
   */
  ~ProgramOptions() = default;

  /**
   * @brief Initialize option descriptions with help message and optional positional option.
   * @param helpMessage The help message
   * @param positional The name of the positional option, if any
   */
  ProgramOptions(const std::string& helpMessage, const std::string& positional = "") :
      m_named { helpMessage + "\n\nSpecific options" }, m_add { m_named.add_options() }, m_positional {} {
    if (positional.length() > 0) {
      m_positional.add(positional.c_str(), -1);
    }
  }

  static ProgramOptions fromAuxdir(const std::string& helpFile, const std::string& positional = "") {
    std::ifstream ifs(Elements::getAuxiliaryPath(helpFile).string());
    std::string helpMessage((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    return ProgramOptions(helpMessage, positional);
  }

  void add(const char* name, const boost::program_options::value_semantic* value, const char* description) {
    m_add(name, value, description);
  }

  std::pair<OptionsDescription, PositionalOptionsDescription> asPair() const {
    return std::make_pair(m_named, m_positional);
  }

private:
  /**
   * @brief Named options description.
   */
  OptionsDescription m_named;

  /**
   * @brief Functor to add options.
   */
  boost::program_options::options_description_easy_init m_add;

  /**
   * @brief Positional options description.
   */
  PositionalOptionsDescription m_positional;

}; // End of Program class

} // namespace FitsIO
} // namespace Euclid

#endif
