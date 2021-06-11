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
 * \verbatim Program <positional> --named1 <value1> --named2 <value2> \endverbatim
 * 
 * Let's assume that the help message is provided in a file help.txt in the auxiliary directory.
 * 
 * In the associated Elements::Program, override defineProgramArguments() as follows:
 * \code
 * std::pair<OptionsDescription, PositionalOptionsDescription> defineProgramArguments() override {
 *   auto options = ProgramOptions::fromAuxdir("help.txt");
 *   options.positional("positional", value<std::string>(), "Positional option");
 *   options.named("named1", value<int>(), "Named option 1");
 *   options.named("named2", value<int>(), "Named option 2");
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
   * @brief Shortcut to Boost's class.
   */
  using ValueSemantics = boost::program_options::value_semantic;

  /**
   * @brief Destructor.
   */
  ~ProgramOptions() = default;

  /**
   * @brief Create option descriptions with help message and optional positional option.
   * @param helpMessage The help message
   * @param positional The name of the positional option, if any
   */
  ProgramOptions(const std::string& helpMessage);

  /**
   * @brief Create option descriptions from help file and optional positional option.
   * @param helpFile The path to the help file relative to the auxiliary directory
   * @details
   * The help file is a file which contains the text to be used as the help message.
   */
  static ProgramOptions fromAuxdir(const std::string& helpFile);

  /**
   * @brief Add a named option.
   * @param name The option name
   * @param value The option value semantics
   * @param description The option description
   */
  void named(const char* name, const ValueSemantics* value, const char* description);

  /**
   * @brief Add a positional option.
   * @param name The option name
   * @param value The option value semantics
   * @param description The option description
   * @param maxArgs The maximum number of option values, or -1 for unlimited
   * @details
   * A positional option is also a named option (thus the name parameter).
   * For example, a positional option `input` can be used either as:
   * \verbatim Program <value> \endverbatim
   * or
   * \verbatim Program --input <value> \endverbatim
   */
  void positional(const char* name, const ValueSemantics* value, const char* description);

  /**
   * @brief Get the named and positional option descriptions.
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> asPair() const;

private:
  /**
   * @brief Named options description.
   */
  OptionsDescription m_namedDesc;

  /**
   * @brief Functor to add named options.
   */
  boost::program_options::options_description_easy_init m_add;

  /**
   * @brief Positional option description.
   */
  PositionalOptionsDescription m_positionalDesc;

}; // End of Program class

} // namespace FitsIO
} // namespace Euclid

#endif
