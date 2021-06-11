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
 * @details
 * Here is an example use case for the following command line:
 * \verbatim
 * Program <positional> --named1 <value1> --named2 <value2>
 * \endverbatim
 * 
 * Let's assume that the help message is provided in a file help.txt in the auxiliary directory.
 * 
 * In the associated Elements::Program, override defineProgramArguments() as follows:
 * \code
 * std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
 *   auto options = ProgramOptions::fromAuxdir("help.txt", "input");
 *   options.add(options.positional(), value<std::string>(), "Input file");
 *   options.add("named1", value<int>(), "Named option 1");
 *   options.add("named2", value<int>(), "Named option 2");
 *   return options.asPair();
 * }
 * \endcode
 */
class ProgramOptions {

public:
  /**
   * @brief Same alias as Elements::Program::OptionsDescription.
   */
  using OptionsDescription = boost::program_options::options_description;

  /**
   * @brief Same alias as Elements::Program::PositionalOptionsDescription.
   */
  using PositionalOptionsDescription = boost::program_options::positional_options_description;

  /**
   * @brief Destructor.
   */
  ~ProgramOptions() = default;

  /**
   * @brief Create option descriptions with help message and optional positional option.
   * @param helpMessage The help message
   * @param positional The name of the positional option, if any
   */
  ProgramOptions(const std::string& helpMessage, const std::string& positional = "") :
      m_namedDesc { helpMessage + "\n\nSpecific options" }, m_add { m_namedDesc.add_options() },
      m_positionalName { positional }, m_positionalDesc {} {
    if (positional.length() > 0) {
      m_positionalDesc.add(positional.c_str(), -1);
    }
  }

  /**
   * @brief Create option descriptions from help file and optional positional option.
   * @param helpFile The path to the help file relative to the auxiliary directory
   * @param positional The name of the positional option, if any
   */
  static ProgramOptions fromAuxdir(const std::string& helpFile, const std::string& positional = "") {
    std::ifstream ifs(Elements::getAuxiliaryPath(helpFile).string());
    std::string helpMessage((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    return ProgramOptions(helpMessage, positional);
  }

  /**
   * @brief Get the name of the positional option.
   */
  const char* positional() const {
    return m_positionalName.c_str();
  }

  /**
   * @brief Add a named option.
   */
  void add(const char* name, const boost::program_options::value_semantic* value, const char* description) {
    m_add(name, value, description);
  }

  /**
   * @brief Get the named and positional option descriptions.
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> asPair() const {
    return std::make_pair(m_namedDesc, m_positionalDesc);
  }

private:
  /**
   * @brief Named options description.
   */
  OptionsDescription m_namedDesc;

  /**
   * @brief Functor to add options.
   */
  boost::program_options::options_description_easy_init m_add;

  /**
   * @brief Positional option name.
   */
  std::string m_positionalName;

  /**
   * @brief Positional option description.
   */
  PositionalOptionsDescription m_positionalDesc;

}; // End of Program class

} // namespace FitsIO
} // namespace Euclid

#endif
