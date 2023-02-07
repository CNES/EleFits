// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITSUTILS_STRINGUTILS_H
#define _ELEFITSUTILS_STRINGUTILS_H

#include "ElementsKernel/Auxiliary.h"

#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace Fits {

/**
 * @brief %String-related utilities.
 */
namespace String {

/**
 * @brief Split a string given a set of character delimiters.
 * @param input The input string
 * @param delimiters The set of delimiters, each one being a single character
 */
std::vector<std::string> split(const std::string& input, const std::string& delimiters = "\n");

/**
 * @brief Trim a string given a set of characters.
 * @param input The input string
 * @param characters The set characters to be trimmed, or `""` for all white spaces
 */
std::string trim(const std::string& input, const std::string& characters = "");

/**
 * @brief Read a text file.
 * @param filename The path to the file
 */
std::string readFile(const std::string& filename);

/**
 * @brief Read a text file from the auxiliary directory.
 * @param filename The path to the file, relative to the auxiliary directory
 */
std::string readAuxFile(const std::string& filename);

/**
 * @brief Copy a `std::string` into a `std::unique_ptr<char[]>`.
 * @details
 * Internally used to work around non-const correctness of CFITSIO:
 * when a function expects a `char`* instead of a `const char*`,
 * `string::c_str()` cannot be used.
 * @see `CStrArray` for multiple strings
 * @par_example
 * To call some function `f(char *)` with a string `s`, do:
 * \code
 * f(toCharPtr(s).get());
 * \endcode
 */
std::unique_ptr<char[]> toCharPtr(const std::string& str);

/**
 * @brief A helper structure to safely convert a `std::vector<std::string>` to `char**`.
 * @see `toCharPtr` for a single string
 * @par_example
 * To call some function `f(char**)` with a vector of strings `v`, do:
 * \code
 * CStrArray a(v);
 * f(a.data());
 * \endcode
 * @warning
 * The `CStrArray` owns the data, and must therefore not be destroyed before the user function ends.
 */
struct CStrArray {

  /// @group_construction

  /**
   * @brief Create from begin and end iterators.
   */
  template <typename T>
  CStrArray(const T begin, const T end);

  /**
   * @brief Create from a vector.
   */
  explicit CStrArray(const std::vector<std::string>& data);

  /**
   * @brief Create from an initializer_list.
   */
  CStrArray(const std::initializer_list<std::string>& data); // FIXME explicit?

  /// @group_properties

  /**
   * @brief The number of elements.
   */
  std::size_t size() const;

  /// @group_elements

  /**
   * @brief Get the data as a non-const `char **`.
   */
  char** data();

  /// @}

  /**
   * @brief A vector of smart pointers to `char[]`.
   * @warning
   * Modification makes CStrArray object invalid.
   */
  std::vector<std::unique_ptr<char[]>> smartPtrVector;

  /**
   * @brief A vector of `char*`.
   * @warning
   * Modification makes `CStrArray` object invalid.
   */
  std::vector<char*> cStrVector;
};

} // namespace String
} // namespace Fits
} // namespace Euclid

#define _ELEFITSUTILS_STRINGUTILS_IMPL
#include "EleFitsUtils/impl/StringUtils.hpp"
#undef _ELEFITSUTILS_STRINGUTILS_IMPL

#endif
