// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_UTILS_PROGRAM_H
#define _ELEFITS_UTILS_PROGRAM_H

#include <boost/program_options.hpp>

namespace Euclid {
namespace Fits {

/**
 * @brief Helper class to declare positional and named options, and help message.
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
   * @brief Create option descriptions with help message.
   * @param helpMessage The help message
   */
  ProgramOptions(const std::string& helpMessage = "");

  /**
   * @brief Create option descriptions from help file.
   * @param helpFile The path to the help file relative to the auxiliary directory
   * @details
   * The help file is a file which contains the text to be used as the help message.
   */
  static ProgramOptions fromAuxFile(const std::string& helpFile);

  /**
   * @brief Add a named option.
   * @param name The option name
   * @param value The option value semantics
   * @param description The option description
   */
  void named(const char* name, const ValueSemantics* value, const char* description);

  /**
   * @brief Add a named option.
   */
  template <typename T>
  void named(const char* name, const char* description) {
    named(name, boost::program_options::value<T>(), description);
  }

  /**
   * @brief Add a named option with default value.
   */
  template <typename T>
  void named(const char* name, const char* description, T defaultValue) {
    named(name, boost::program_options::value<T>()->default_value(defaultValue), description);
  }

  /**
   * @brief Add a positional option.
   * @param name The option name
   * @param value The option value semantics
   * @param description The option description
   * @details
   * A positional option is also a named option (thus the name parameter).
   * For example, a positional option `input` can be used either as:
   * \verbatim Program <value> \endverbatim
   * or
   * \verbatim Program --input <value> \endverbatim
   * 
   * Positional options should be added in the expected order of the command line.
   */
  void positional(const char* name, const ValueSemantics* value, const char* description);

  /**
   * @brief Add a positional option.
   */
  template <typename T>
  void positional(const char* name, const char* description) {
    positional(name, boost::program_options::value<T>(), description);
  }

  /**
   * @brief Add a positional option with default value.
   */
  template <typename T>
  void positional(const char* name, const char* description, T defaultValue) {
    positional(name, boost::program_options::value<T>()->default_value(defaultValue), description);
  }

  /**
   * @brief Add a flag.
   * @param name The flag name
   * @param description The flag description
   * @details
   * A flag is a named option which is either set (implicitely to `true`)
   * or omitted, in which case the value is `false`.
   * A flag is simply set as:
   * \verbatim Program --flag \endverbatim
   */
  void flag(const char* name, const char* description);

  /**
   * @brief Get the named and positional option descriptions.
   * @details
   * This methods is primarily intended as the return of the override
   * of Elements::Program::defineProgramArguments().
   */
  std::pair<OptionsDescription, PositionalOptionsDescription> asPair() const;

private:
  /**
   * @brief Build the options description from a possibly empty help message.
   */
  static std::string makeDesc(const std::string& helpMessage);

  /**
   * @brief Named options description.
   */
  OptionsDescription m_named;

  /**
   * @brief Functor to add named options.
   */
  boost::program_options::options_description_easy_init m_add;

  /**
   * @brief Positional option description.
   */
  PositionalOptionsDescription m_positional;

}; // End of Program class

} // namespace Fits
} // namespace Euclid

#endif
